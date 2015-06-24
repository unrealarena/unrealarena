[![Build Status](https://travis-ci.org/unrealarena/unrealarena.svg?branch=master)](https://travis-ci.org/unrealarena/unrealarena) [![Build status](https://ci.appveyor.com/api/projects/status/a1dwc3xr7jottyls/branch/master?svg=true&pendingText=pending&failingText=failing&passingText=passing)](https://ci.appveyor.com/project/unrealarena/unrealarena)

# Unreal Arena

Unreal Arena is a fast-paced first-person shooter that aims to merge the
universes of Unreal Tournament** and Quake III Arena**. The idea behind it is
very simple: the player can choose from either side, determining in this way
which weapons/items/movements are available, and fight against other players of
the opposite faction with their own traits.

The game is a community developed open-source project powered by the
[Daemon](http://unvanquished.net) engine and has all content recreated from
scratch inspired by the original works.

** *Unreal Tournament™ is a trademark or registered trademark of Epic Games,
Inc. in the United States of America and elsewhere. Quake III Arena™ is a
trademark or registered trademark of id Software LLC in the United States and/or
other countries. The authors of Unreal Arena are not affiliated with, endorsed
by, nor connected in any way to Epic Games, Inc., id Software LLC or any of
theirs subsidiaries.*


## Build Instructions


### Linux

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make -j$(nproc)
```


### OS X

*TODO*


### Windows

*TODO*


#### 32-bit cross-compiling from Linux

```bash
$ mkdir build-win32
$ cd build-win32
$ cmake -DCMAKE_TOOLCHAIN_FILE=cmake/cross-toolchain-mingw32.cmake ..
$ make -j$(nproc)
```


#### 64-bit cross-compiling from Linux

```bash
$ mkdir build-win64
$ cd build-win64
$ cmake -DCMAKE_TOOLCHAIN_FILE=cmake/cross-toolchain-mingw64.cmake ..
$ make -j$(nproc)
```


## Run Instructions

*TODO*
