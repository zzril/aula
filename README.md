aula
====

Audio Language

This is my attempt of creating an audio programming language from scratch, using only the C standard lib and SDL2.

The interpreter is currently working in the sense that we can pass it an aula source file and it will play the notes listed in the file.  
However, pretty much everything else is missing atm. There is no language documentation yet, no examples, and error reporting is as rudimentary as it gets.  
Features are also extremely limited for now. We can currently play all (half) notes from C4 to B4. And the sound is not particularly interesting we're merely emitting a simple sine wave.

Usage
-----
```sh
make
./bin/aula <path to aula source file>
```

