aula
====

Audio Language

This is my attempt of creating an audio programming language from scratch, using only the C standard lib and SDL2.

The interpreter is currently working in the sense that we can pass it an aula source file and it will play the notes listed in the file.  
However, pretty much everything else is missing atm. There is no language documentation yet, no examples, and error reporting is as rudimentary as it gets.  
Features are also extremely limited for now. We can currently play all (half) notes from C4 to B4. And the sound is not particularly interesting we're merely emitting a simple sine wave.

Build
-----

First, install the dependencies. On Debian-based Linux distros, this will do:  
```sh
sudo apt install libsdl2-dev
```

Then compile the interpreter by typing:  
```sh
make
```

Usage
-----

To play a file, start the interpreter and give the filename as an argument:  
```sh
./bin/aula examples/canon-in-d.aula
```

An aula file could currently look for example like this:  
```
|| D A B F# | G D G A ||
```

(See more examples in the `examples` folder.)

