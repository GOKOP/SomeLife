Simulates interactions between particles, sometimes called "particle life".

# Build

This program depends on the SFML graphical library and uses CMake for the build process.
Note that directories `res` and `recipes` talked about in the "Usage" section are copied to the build directory for convenience.

## Linux

1. Install SFML from your distro's package manager.
2. Run the following commands:
```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```
Make sure to specify the build type, as CMake's default (Debug) is unoptimized and runs very slowly.

## Windows (using vcpkg and Visual Studio)

The simplest way to get SFML on Windows and have it recognized by CMake is to use [vcpkg](https://vcpkg.io/en/index.html).
You can use Visual Studio's built-in support for CMake projects (although you may need to install it).

1. If you don't have vcpkg, [follow these instructions](https://vcpkg.io/en/getting-started.html)
Vcpkg seems to install x86 libraries by default, so unless you're going to compile for x86, install SFML with the following command:
```
./vcpkg.exe install SFML --triplet x64-windows
```
3. In Visual Studio, select the "open local folder" option and select the project directory.
4. Go to "manage configurations" and add a configuration "x64-Release". Debug build is unoptimized and thus very slow.
5. Select Project > Configure Cache
6. Switch to the "x64-Release" configuration, choose the "somelife.exe" target and build it.