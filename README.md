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

Another useful new instruction is `dump` with takes no argument. `dump` dumps whole memory (whole program) to stdout. Example:

```
< @PC = 60 @A = 0 @B = 0 >
0:    ----  @A 0       [16        ]
4:    store @A 0       [3         ]
8:    null  @A 0       [0         ]
12:   ----  @A 0       [15        ]
16:   load  @A (8)     [528386    ]
20:   jzero @A 52      [3407877   ]
24:   load  @A (12)    [790530    ]
28:   add   @A (4)     [266249    ]
32:   store @A 12      [786435    ]
36:   load  @A (8)     [528386    ]
40:   sub   @A 1       [65546     ]
44:   store @A 8       [524291    ]
48:   jump  @A 20      [1310724   ]
52:   print @A (12)    [790797    ]
56:   dump  @A 0       [14        ]
60:   halt  @A 0       [1         ]
```

The first line contains register values. The following lines contain word dumps. Each word is interpreted both as instruction and as data. For example, line `4:    store @A 0       [3         ]` means that the word at address `4` contains value `3` which is `store @A 0` when interpreted as an instruction.




