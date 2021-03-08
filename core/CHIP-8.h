#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

#define MEMORY_FONTSET_START 0x000
#define MEMORY_PGM_START 0x200
#define FONTSET_SIZE 80
#define VIDEO_SIZE 64 * 32
#define NUM_KEYS 16

#define ISA_SIZE 35

struct CHIP8_s;
typedef struct CHIP8_s CHIP8;

typedef void (*instruction_runner)(CHIP8 *, uint16_t);

/**
 * Instructions available on the CHIP8 architecture
 * are represented by means of an instruction_s struct
 * and stored in the ISA field of the CHIP8_s struct.
 *
 * When a new opcode is fetched from memory, the
 * emulator scans the ISA array looking for a matching
 * instructions. More in details, an opcode O and an instruction I
 * match if the result of the bitwise AND between O and I.mask is
 * equal to I.opcode. When a match is found, the corresponding
 * action is executed.
 */
struct instruction_s {
    uint16_t mask;
    uint16_t opcode;

    // pointer to the function that executes the instruction
    instruction_runner execute;
};

typedef struct instruction_s instruction_t;

struct CHIP8_s {
    /**
     * CHIP-8 can address 4 KB of memory.
     *
     * Memory map:
     *  - [0x000, 0x1FF]: reserved for the CHIP-8 interpreter
     *  - 0x200: start of most CHIP-8 programs
     *  - 0x600: start of ETI 660 CHIP-8 programs
     *  - [0x200, 0xFFF]: CHIP-8 program/data space
     */
    uint8_t memory[4096];

    /**
     * 16 general purpose 8-bit register
     * registers are referred as Vx (x is an hex digit between 0 and F)
     */
    union register_file {
        uint8_t raw[16];
        struct {
            uint8_t V0;
            uint8_t V1;
            uint8_t V2;
            uint8_t V3;
            uint8_t V4;
            uint8_t V5;
            uint8_t V6;
            uint8_t V7;
            uint8_t V8;
            uint8_t V9;
            uint8_t VA;
            uint8_t VB;
            uint8_t VC;
            uint8_t VD;
            uint8_t VE;
            uint8_t VF;
        };
    } register_file;

    /**
     * Index register
     */
    uint16_t I;

    /**
     * Program Counter
     */
    uint16_t PC;

    /**
     * Video memory
     * CHIP8 has a 64x32 monochromatic screen. A lit pixel
     * is represented as a nonzero value in the video matrix.
     * Pixels are stored rows first.
     */
    uint8_t video[64 * 32];

    /**
     * Timers
     * CHIP8 has two timer that count towards zero at 60 Hz.
     */
    uint8_t delay_timer;
    uint8_t sound_timer;

    /**
     * Stack: 16 16-bit values
     */
    uint16_t stack[16];
    uint8_t SP;

    /**
     * Keyboard
     * CHIP8 has a 16 keys keyboard
     */
    uint8_t key[16];

    /**
     * Draw flag: when this flag is set to a nonzero value
     * the screen needs to be refreshed.
     */
    uint8_t draw_flag;

    /**
     * Instruction set
     */
    instruction_t ISA[ISA_SIZE];

    /**
     * Function called by the emulator in order to update the screen.
     * @param video is a pointer to the matrix that stores the screen content
     */
    void (*refresh_screen)(const uint8_t *video);

    /**
     * Function called by the emulator in order to emit a BEEP.
     */
    void (*beep)();

    /**
     * Function called by the emulator in order to read the keyboard status.
     */
    void (*keyboard_input)(uint8_t *keyboard);
};

/**
 * Initialize the CHIP8 emulator and load the provided rom in main memory.
 *
 * @param chip8 is a pointer to the CHIP8 struct
 */
extern void CHIP8_init(CHIP8 *chip8);

/**
 * Load a ROM in the CHIP8 memory from file.
 *
 * @param chip8 is a pointer to the CHIP8 struct
 * @param rom is the path of the rom
 */
extern void CHIP8_load_rom_from_file(CHIP8 *chip8, char *path);

/**
 * Load a ROM in the CHIP8 memory.
 *
 * @param chip8 is a pointer to the CHIP8 struct
 * @param rom is the rom's content
 * @param length is the the size of the rom
 */
extern void CHIP8_load_rom_bytes(CHIP8 *chip8, uint8_t *rom, int length);

/**
 * Set the function called by the emulator in order to update the screen.
 *
 * @param chip8 is a pointer to the CHIP8 struct
 * @param refresh is a pointer to the function
 */
extern void CHIP8_set_refresh_function(CHIP8 *chip8, void (*refresh)(const uint8_t *));

/**
 * Set the function called by the emulator in order to emit a beep.
 *
 * @param chip8 is a pointer to the CHIP8 emulator
 * @param beep is a pointer to the function
 */
extern void CHIP8_set_beep_function(CHIP8 *chip8, void (*beep)());

/**
 * Set the function called by the emulator in order to read the keyboard.
 *
 * @param chip8 is a pointer to the CHIP8 emulator
 * @param keyboard_input is a pointer to the function
 */
extern void CHIP8_set_keyboard_input_function(CHIP8 *chip8, void (*keyboard_input)(uint8_t *));

/**
 * Main CPU loop: fetch, decode, execute and repeat.
 * @param chip8 is a pointer to the emulator
 */
_Noreturn extern void CHIP8_loop(CHIP8 *chip8);

/**
 * Emulate one CPU cycle
 * @param chip8 is a pointer to the emulator
 */
extern void CHIP8_tick(CHIP8 *chip8);

#endif