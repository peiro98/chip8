# CHIP8

A dead simple and low effort CHIP 8 emulator.

![Pong running on the CHIP8 emulator](pong.png)

## Usage
```bash
gcc main.c core/*.c core/*.h -lncurses -o CHIP8
./CHIP8 pong.c8
```

## References
 - [General introduction to CHIP8 emulators](http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/)
 - [Technical reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)

## License
[MIT](https://choosealicense.com/licenses/mit/)