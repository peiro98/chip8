terminal: main.c ./core/*.c ./core/*.h
	gcc main.c core/*.c core/*.h -lncurses -o CHIP8.out


clean_terminal:
	rm -rf *.out


web: chip8_wasm.c ./core/*.c ./core/*.h
	emcc -o web/chip8.js chip8_wasm.c  core/*.c -Os -s WASM=1 -s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap"]' -s NO_FILESYSTEM=1


clean_web:
	rm web/chip8.js web/chip8.wasm