aula
====

Audio Language

![](https://github.com/zzril/aula/actions/workflows/c-cpp.yml/badge.svg)

This is my attempt of creating an audio programming language from scratch, using only the C standard lib and SDL2.

As this is still pretty new, don't expect too many features yet. For now, it's possible to write a note file in an ABC-like notation and have that played by the interpreter.  
Sound is just a simple sine wave so far. A feature to define more interesting sounds, as well as multiple instruments, is planned.  
Documentation is almost nonexistent at this point, and error reporting is... let's call it "basic".  
Needless to say, the (nonexistent) language specification should not be considered anything close to "stable".

Help is appreciated! More specific information on how to contribute will follow.

Build
-----

First, you need to install the dependencies.  
On Debian-based Linux distros that would be achieved by:  
```sh
sudo apt update
sudo apt install libsdl2-dev
```

Compiling is done by invoking `make`:  
```sh
make
```

A test target also exists:  
```sh
make check
```

When testing locally, running multiple tests in parallel via the `-j` option is not recommended as the sound will overlap.

Usage
-----

To play a file, start the interpreter and give the filename as an argument:  
```sh
./bin/aula examples/example1.aula
```

An aula file could currently look for example like this:  
```
bpm: 40

track:
|| D+ A B F# | G D G A ||
```

(See more examples in the `examples` folder.)

