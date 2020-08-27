# HexDump

![CMake](https://github.com/TheEvilRoot/hex-dump-utility/workflows/CMake/badge.svg)

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

First line is indexing header. Every line contains 16 bytes and represented by 2-digit hex values.
Every line is numirated by index of first byte of the line in file. 

You can get byte's position in file by adding line number to byte's index in the line (and byte's index in the file by subtracting 1 from position value).

##### Example

```
   Index 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16
       0 68 65 6c 6c 6f 0d 68 65 6c 6c 6f 0d 68 65 6c 6c
      16 6f 0d 68 65 6c 6c 6f 0d 68 65 6c 6c 6f 20 77 6f
      32 72 6c|64|20 68 65 6c 6c 6f 0d 68 65 6c 6c 6f 20
      48 77 6f 72 6c 64 20 68 65 6c 6c 6f ffffffff
```

To get index of byte `64` in the file, you need to add 32 (line number) to 3 (byte's index) - `32 + 3 = 35` - 35 is position of byte `64`, so it's index is `34`

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
