# A readable micro kernel operating system.
![](https://img.shields.io/badge/Albatross-Shmily-lightgrey)
![](https://img.shields.io/badge/license-GPL%203.0-blue)
![](https://img.shields.io/badge/continue-updating-green)

## Introduction

- 32bit
- Micro kernel
- Readable, try to use C++'s code rather than assembly code.
- Using C++, of course we couldn't use the runtime lib.

## Environment

`sudo apt-get install -y bochs`

`sudo apt-get install -y boch-x`

`sudo apt-get install -y g++`

`sudo apt-get install -y nasm`

If you want running in virtual box or qemu, install corresponding environment.

-------

## Run
` make DEBUGGER=1` or `make`

There are some bugs running in virtual box.

And the ne2k can't work in qemu.

So recommend the bochs.

## Recent Plans
1. Improve the network module.
2. When we complete the work above, we have a more complete operating system kernel. Then we can add some algorithm for mm,fs..., and improve the code's robustness.
3. Tidy up codes.

