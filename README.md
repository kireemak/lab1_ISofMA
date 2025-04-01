# File Access Manager

A cross-platform tool for packaging/unpacking files along with their access permissions into a single archive. Provides a re-application of the permissions of the original file owner.

## Dependencies

- **FLTK (GUI Library)**.
- **libzip**.

### Installation

#### Windows (via vcpkg):
```bash
vcpkg install fltk libzip
```

#### macOS (via homebrew):
```bash
brew install fltk libzip
```

## Building

### Windows (via CMake using vcpkg toolchain):
```bash
cmake -B build -DCMAKE_TOOLCHAIN_FILE=[путь_к_vcpkg]/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
```

## Made by
 - Makar Kireenko, Chair of Programming Technologies, group 12, year 3