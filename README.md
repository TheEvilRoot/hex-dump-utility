# HexDump

HexDump is a simple utility program written in C++ to get hexdump of text or binary file.

#### Sample of output

```
   Index 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16
       0 68 65 6c 6c 6f 0d 68 65 6c 6c 6f 0d 68 65 6c 6c
      16 6f 0d 68 65 6c 6c 6f 0d 68 65 6c 6c 6f 20 77 6f
      32 72 6c 64 20 68 65 6c 6c 6f 0d 68 65 6c 6c 6f 20
      48 77 6f 72 6c 64 20 68 65 6c 6c 6f ffffffff

Length: 60
Lines: 5
Type of line separator: LF (0D)
```

### Usage

```bash

$ HexDump file.txt

```

### Building

I think, HexDump *is* fully crossplatform because it's implemented only with C++17 STL (such `filesystem`, `variant`, `fstream`)

Visual Studio
-------------

With Visual Studio you only need to clone or download this repo and open VS solution. 

> Make sure that C++17 is enabled in configurations

Other platforms
---------------

You can build HexDump on every platform that supported by CMake using CMakeLists.txt. 

- First, you need to download or clone this repo
- Create build directory (e.g. `mkdir cmake-build`) and navigate to it (e.g. `cd cmake-build`)
- Init CMake project (`cmake ..`)
- If process was successfull, navigate down to project's root directory and build project using cmake
- `cmake --build cmake-build --target all`
- Get builded binaries in build directory
