# aghsm

## Overview

This projects aims to implement a DC2 (Didactic Computer with 2 accumulators). Both assembler and virtual machine are provided.

## Cloning, building and running

### Linux / OS X

Install CMake.

`cd` to a desired location

`git clone https://github.com/cubuspl42/aghsm.git`

`mkdir build`

`cd build`

`cmake ..`

`make`

`./aghsm /path/to/source.txt`

Program will be immedietaly assembled and executed.

### Windows

Instead of using `make`, you can generate a Visual Studio solution using CMake and build it.

