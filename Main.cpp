#include <filesystem>
#include <iostream>
#include <string>
#include <variant>
#include <optional>
#include <functional>
#include <fstream>
#include <iomanip>

struct Options
{
	typedef std::variant<Options, std::string> Result;
	typedef std::optional<std::string> Error;

	std::string file_path;
	bool divide_by_lines = false;
	int max_lines = -1;

	[[nodiscard]]
	Error check_options() const
	{
		if (file_path.empty())
		{
			return Error("File path is empty");
		}
		if (!std::filesystem::exists(file_path))
		{
			return Error("File " + file_path + " doesn't exists");
		}
		if (std::filesystem::is_directory(file_path))
		{
			return Error(file_path + " is directory");
		}
		return Error();
	}

	static Result from_args(int argc, const char* argv[])
	{
		if (argc > 1)
		{
			Options opt{ "" };
			for (int i = 1; i < argc; i++)
			{
				std::string arg(argv[i]);
				if (arg.empty()) continue;
				if (arg.front() == '-')
				{
					if (arg == "-L")
					{
						opt.divide_by_lines = true;
					}
					else if (arg == "-n")
					{
						if (i + 1 >= argc)
						{
							return Result("Expected parameter {count} after -n");
						}
						std::string value(argv[++i]);
						try 
						{
							opt.max_lines = std::stoi(value);
						} 
						catch (...)
						{
							return Result("Expected number after -n");
						}
					}
					else 
					{
						return Result("Unknown parameter: " + arg);
					}
				}
				else
				{
					if (opt.file_path.empty())
					{
						opt.file_path = arg;
					}
					else
					{
						return Result("Ambiguous arguments: file is " + opt.file_path + " or " + arg);
					}
				}
			}
			return Result(opt);
		}
		return Result("Usage: " + std::string(argv[0]) + " <file>");
	}
};

enum class LiningType
{
	CR, LF, CRLF, MIXED, UNDEFINED
};

std::string get_type_name(LiningType type)
{
	switch (type)
	{
	case LiningType::CRLF: return "CRLF (0D 0A)";
	case LiningType::LF: return "LF (0D)";
	case LiningType::CR: return "CR (0A)";
	case LiningType::MIXED: return "Mixed types";
	default: return "Undefined";
	}
}

struct LineCounter
{
	LiningType type = LiningType::UNDEFINED;
	bool has0D = false;
	size_t state = 0;
	bool new_line_flag = false;

	void update_type(LiningType t)
	{
		if (type == LiningType::UNDEFINED)
		{
			type = t;
		}
		else if (type != t)
		{
			type = LiningType::MIXED;
		}
	}

	[[nodiscard]]
	size_t acquire_state()
	{
		size_t s = state;
		state = 0;
		new_line_flag = s != 0;
		return s;
	}

	char handle(char c)
	{
		if (c == 0x0A || has0D)
		{
			state = 1;
			if (c == 0x0A && has0D) update_type(LiningType::CRLF);
			if (c == 0x0A && !has0D) update_type(LiningType::CR);
			if (c != 0x0A && has0D) update_type(LiningType::LF);
			has0D = false;
		}
		if (c == 0x0D && has0D)
		{
			state = 2;
			has0D = false;
			update_type(LiningType::LF);
		}
		if (c == 0x0D && !has0D)
		{
			has0D = true;
		}
		return c;
	}
};

template<typename T, typename R>
std::variant<T, R> guard(const std::variant<T, R>& var, const std::function<std::optional<R>(const T&)>& f)
{
	if (std::holds_alternative<T>(var))
	{
		auto t = std::get<T>(var);
		if (auto r = f(t); r)
		{
			return std::variant<T, R>(r.value());
		}
		return std::variant<T, R>(t);
	}
	return std::variant<T, R>(std::get<R>(var));
}

template<typename T, typename ...R>
std::optional<T> extract(const std::variant<R...>& var)
{
	if (std::holds_alternative<T>(var))
	{
		return std::optional<T>(std::get<T>(var));
	}
	return std::optional<T>();
}

bool should_new_line(const Options& opt, int index, const LineCounter &counter)
{
	if (opt.divide_by_lines)
	{
		return index == 0 || counter.new_line_flag;
	}
	return index % 16 == 0;
}

bool lines_limit_check(const Options& opt, int index, int lines)
{
	return opt.max_lines > 0 && (opt.divide_by_lines && opt.max_lines < lines || !opt.divide_by_lines && opt.max_lines <= index / 16);
}

void hex_dump(const Options& opt)
{
	std::ifstream file(opt.file_path, std::ios::binary);
	LineCounter counter;

	if (!file.is_open())
	{
		std::cerr << "Failed to open file " << opt.file_path << "\n";
		return;
	}

	size_t index = 0;
	size_t lines = 1;

	if (!opt.divide_by_lines) 
	{
		std::cout << std::setw(9) << std::setfill(' ') << "Index ";
		for (int i = 1; i <= 16; i++) 
		{
			std::cout << std::setw(2) << std::setfill('0') << i << " ";
		}
	}

	do {
		if (should_new_line(opt, index, counter))
		{
			std::cout << std::endl;
			std::cout << std::setw(8) << std::setfill(' ') << std::dec << index << " ";
		}
		auto current = counter.handle(file.get());

		std::cout << std::hex << std::setw(2) << std::setfill('0') << int(current & 0xff) << " ";
		index++;
		lines += counter.acquire_state();

		if (lines_limit_check(opt, index, lines))
		{
			break;
		}
	} while (!file.eof());

	std::cout << "\n" << std::endl;
	std::cout << "Length: " << std::dec << index << "\n";
	std::cout << "Lines: " << lines << "\n";
	std::cout << "Type of line separator: " << get_type_name(counter.type) << "\n";

	file.close();
}

int main(int argc, const char* argv[])
{
	auto state = guard<Options, std::string>(Options::from_args(argc, argv), &Options::check_options);
	if (auto error = extract<std::string>(state); error)
	{
		std::cerr << error.value() << "\n";
		return 1;
	}
	if (auto opt = extract<Options>(state); opt)
	{
		hex_dump(opt.value());
	}
}