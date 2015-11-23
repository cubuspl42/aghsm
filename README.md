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

Program will be immedietaly assembled and executed

### Windows

Visit [Releases](https://github.com/cubuspl42/aghsm/releases) page

Download `aghsm.exe`

Put it anywhere you like (for example "C:\aghsm\aghsm.exe")

Put your source file in the same directory

Launch cmd.exe

`cd C:\aghsm\`

`aghsm.exe sourcefile.txt`

Program will be immedietaly assembled and executed

## Extensions

In order to allow users to see the output of their program, `print` instruction was added

`print, @A` or

`print, @B` or

`print, (x)`




