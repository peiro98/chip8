terminal: main.c ./core/*.c ./core/*.h
	gcc main.c core/*.c core/*.h -lncurses -o CHIP8.out


clean:
	rm -rf *.out