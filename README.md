# Intermittent-safe example program for MSP430

Dummy example running on MSP430, intermittent-safe. It makes use of a custom, simple library developed by us based on the [_Chain_](https://brandonlucia.com/pubs/chain.pdf) idea (paper by A. Colin and B. Lucia).

## How to use

Just clone/download the repository and import the project in CCS, build and run.

## Program

The program takes 4 input arrays, then:

1. finds the minimum of each array;
2. subtracts each array's minimum to each array's element;
3. computes the average of each array.

A first task executes point 1, and a second task executes points 2 and 3.
The first task informs the second task about each array's minimum. The latter computes subtraction and average.

The library ensures that, when a task reads some data coming from another task (or from itself), such data is consistent and immune to power failures.

## Suggestions

This dummy program is fairly useless. Its purpose is to show you how to use the intermittent-safe library we provide you, in case you want to. Its usage is quite simple, but some steps have to be followed to make it work. Look at the code, it has some useful comments. Moreover, after cloning the repository, open `interpow/doc/html/index.html` for an extensive documentation.

## Known issues

#### Compiler version
The project was built and tested with the latest MSP430 compiler version (17.9.0). If you don't have it, please download it from CCS by going to `Help > Install New Software` and typing "Code Generation Tools Updates" in the search bar. On Linux distributions you may have to run CCS as a superuser, e.g. in Ubuntu
```bash
sudo /opt/ti/ccsv7/eclipse/ccstudio
```

#### Compiler flags
When importing the CCS project, some systems may cut out some of the compiler flags needed to build this project without errors. To make sure all your compiler flags are set correctly, have a look at [this list](https://github.com/phdschooltpc/msp430-intermittent-example/blob/master/list_of_compiler_flags.md) and compare it with your project's compiler settings in `Project > Properties > CCS Build`.
