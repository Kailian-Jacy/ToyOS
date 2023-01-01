# ToyOS for ZJU OSlab 2022

Used only for study purpose. DO NOT USE FOR CHEATING HOMEWORK.

## Overview

This is a simple unix style toy operating system developed in riscv. It's used for oslab 2022 by now.

This involves:
- Loading and booting from `opensbi`. (lab1)
- Basic interrupt implementation. (lab2) 
- Multi-process and multithread switching. (lab3)
- Page table and virtual memory. (lab4)
- User mode process and page table isolation. (lab5)
- Page fault and demand-paging. (lab5)
- `SYS_CLONE` systemcall and `fork()`. (lab5)

## Running

Run this natively on riscv64:

```
cd lab[1-5]
make run
```

If you are running from other arch, you may want to install the following cross-compiler:
- https://github.com/riscv-collab/riscv-gnu-toolchain