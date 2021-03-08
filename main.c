#include <unistd.h>
#include <curses.h>

#include "core/CHIP-8.h"

void window_setup();

void refresh_screen(const uint8_t *video);

void emit_beep();

void keyboard_input(uint8_t *keyboard);

int main(int argc, char **argv) {
    CHIP8 chip8;

    if (argc != 2) {
        fprintf(stdout, "USAGE: ./chip8 /path/to/rom");
        return 1;
    }

    window_setup();

    // CHIP8_init the CHIP-8 emulator with the user-specified rom
    CHIP8_init(&chip8);
    CHIP8_load_rom_from_file(&chip8, argv[1]);
    CHIP8_set_refresh_function(&chip8, &refresh_screen);
    CHIP8_set_beep_function(&chip8, &emit_beep);
    CHIP8_set_keyboard_input_function(&chip8, &keyboard_input);

    // main loop emulating the cpu
    CHIP8_loop(&chip8);
}

/**
 * Initialize the curses window
 */
void window_setup() {
    WINDOW *window;
    window = initscr();
    noecho();
    nodelay(window, TRUE);
    cbreak();
    curs_set(FALSE);
}

/**
 * Update the screen with the content of the video memory
 * of the emulator. The CHIP8 has a 64x32 monochromatic
 * screen.
 *
 * Currently, the screen is emulated using a curses window.
 * Lit pixels are represented with a '0' character. Since refreshing
 * the whole window is slow, this function tracks the previous
 * state of the window in the 'old' static variable.
 *
 * @param video is a pointer to the matrix the contains the video state
 */
void refresh_screen(const uint8_t *video) {
    static int frame_counter = 0;
    static uint8_t old[VIDEO_SIZE] = {0};

    frame_counter++;

    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            if (old[y * 64 + x] ^ video[y * 64 + x]) {
                move(y, x);
                addch(video[y * 64 + x] ? '0' : ' ');
                old[y * 64 + x] = video[y * 64 + x];
            }
        }
    }

    refresh();
}

/**
 * Emit a BEEP!
 */
void emit_beep() {
    printf("\a\n");
}

/**
 * Read keyboard status.
 *
 * Since curses can only detect 'key down' events, the pressure
 * of a key activates it until the key is pressed again.
 *
 * Physical keyboard   <-->   CHIP8 keypad
 *
 *   1 2 3 4                    1 2 3 C
 *   q w e r                    4 5 6 D
 *   a s d f                    7 8 9 E
 *   z x c v                    A 0 B F
 *
 * @param keyboard is a pointer to the matrix that represents the keyboard
 */
void keyboard_input(uint8_t *keyboard) {
    char key;

    if ((key = getch()) != ERR) {
        if (key == '1') keyboard[0x1] ^= 1;
        else if (key == '2') keyboard[0x2] ^= 1;
        else if (key == '3') keyboard[0x3] ^= 1;
        else if (key == '4') keyboard[0xC] ^= 1;

        else if (key == 'q') keyboard[0x4] ^= 1;
        else if (key == 'w') keyboard[0x5] ^= 1;
        else if (key == 'e') keyboard[0x6] ^= 1;
        else if (key == 'r') keyboard[0xD] ^= 1;

        else if (key == 'a') keyboard[0x7] ^= 1;
        else if (key == 's') keyboard[0x8] ^= 1;
        else if (key == 'd') keyboard[0x9] ^= 1;
        else if (key == 'f') keyboard[0xE] ^= 1;

        else if (key == 'z') keyboard[0xA] ^= 1;
        else if (key == 'x') keyboard[0x0] ^= 1;
        else if (key == 'c') keyboard[0xB] ^= 1;
        else if (key == 'v') keyboard[0xF] ^= 1;
    }
}