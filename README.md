# SomeLife

Simulates interactions between particles, sometimes called "particle life".
Called "SomeLife" because that's what I've happened to call the directory where I've started messing with the idea.
It's somewhat inspired by [this video](https://www.youtube.com/watch?v=Z_zmZ23grXE), but differs from what's there.

## Build

This program depends on the SFML graphical library and uses CMake for the build process.
It uses OpenMP for parallelism so make sure your compiler supports it; otherwise everything will run on a single thread.
Note that directories `res` and `recipes` talked about in the "Usage" section are copied to the build directory for convenience.

### Linux

1. Install SFML from your distro's package manager.
2. Run the following commands:
```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```
Make sure to specify the build type, as CMake's default (Debug) is unoptimized and runs very slowly.

### Windows (using vcpkg and Visual Studio)

The simplest way to get SFML on Windows and have it recognized by CMake is to use [vcpkg](https://vcpkg.io/en/index.html).
You can use Visual Studio's built-in support for CMake projects (although you may need to install it).

1. If you don't have vcpkg, [follow these instructions](https://vcpkg.io/en/getting-started.html).
Vcpkg seems to install x86 libraries by default, so unless you're going to compile for x86, install SFML with the following command:
```
./vcpkg.exe install SFML --triplet x64-windows
```
2. In Visual Studio, select the "open local folder" option and select the project directory.
3. Go to "manage configurations" and add a configuration "x64-Release". Debug build is unoptimized and thus very slow.
4. Select Project > Configure Cache
5. Switch to the "x64-Release" configuration, choose the "somelife.exe" target and build it.

## Usage

SomeLife is designed to be run from the terminal:

```
./somelife path-to-recipe
```

Where `path-to-recipe` is the path to a "recipe" file defining simulation setup.
Example recipes can be found in the "recipes" directory.
The directory that SomeLife is run from should contain the `res` directory.

### Recipe files

Possible commands are: `window`, `friction`, `particles` and `rule`. 
Comments are indicated by the `#` sign at the beginning of the line. 
The file must start with a `window` command, and must not have more than one such command.
The "recipes" directory contains example recipes as well as a python script to generate random ones.

#### window

```
window width height
```

`width` and `height` are positive integers. 
Defines the size of the window.

#### friction

```
friction value
```

`value` is a floating point number. Defines friction.
It's applied to particle's velocity by multiplying it by `1 - friction`.

#### particles

```
particles color amount
```

`color` is one of: white, red, green, blue, yellow, magenta, cyan.
`amount` is a positive integer.
Adds particles of a given color to the simulation.

#### rule

```
rule color1 color2 first_cut second_cut peak
```
`color1` and `color2` are colors as listed under `particles`.
`first_cut`, `second_cut` and `peak` are floating point numbers.

Defines a rule that describes how particles of `color1` are affected by particles of `color2`.
It does not cause particles of `color1` to have any effect on particles of `color2`.
The rule works as follows:

Force exerted on particles by other particles can be described with a graph:

```
force
  ^
  |
1 --
  | \
  |  | peak
  |  \---
  |      \
  |       ------
  |--|----------\-----------> distance
   first      second
    cut        cut
```

`first_cut` and `second_cut` should be positive, and `second_cut` should be larger or equal than `first_cut`.
`peak` can be positive or negative.
Positive values represent repulsion, negative values represent attraction.
Force before `first_cut` is always positive (repelling).

For any given particle, every rule where its color is `color1` is considered for every surrounding particle of `color2` colors.
Force is added up to velocity and position is updated after considering all relevant rules.

### Config

In the file `res/somelife.conf` you can specify target framerate of the simulation as well as the number of threads.
