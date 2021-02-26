#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "CHIP-8.h"
#include "instructions.h"


#define CLK_PERIOD (1e6 / 60)

uint8_t fontset[FONTSET_SIZE] =
        {
                0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
                0x20, 0x60, 0x20, 0x20, 0x70, // 1
                0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
                0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
                0x90, 0x90, 0xF0, 0x10, 0x10, // 4
                0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
                0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
                0xF0, 0x10, 0x20, 0x40, 0x40, // 7
                0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
                0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
                0xF0, 0x90, 0xF0, 0x90, 0x90, // A
                0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
                0xF0, 0x80, 0x80, 0x80, 0xF0, // C
                0xE0, 0x90, 0x90, 0x90, 0xE0, // D
                0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
                0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };

static void define_instruction(CHIP8 *chip8, int index,
                               uint16_t mask, uint16_t opcode,
                               void (*execute)(CHIP8 *, uint16_t));

void load_ISA(CHIP8 *chip8);

void load_rom(CHIP8 *chip8, char *path);


/**
 * Initialize the CHIP8 emulator and load the provided rom in main memory.
 *
 * @param chip8 is a pointer to the CHIP8 struct
 * @param rom is the path of the rom
 */
void CHIP8_init(CHIP8 *chip8, char *rom) {
    srand((unsigned) getpid());

    // reset current instruction
    chip8->PC = MEMORY_PGM_START;

    chip8->SP = 0;
    chip8->I = 0;
    chip8->sound_timer = 0;
    chip8->delay_timer = 0;

    // load the instruction set
    load_ISA(chip8);

    // load the rom in main memory
    load_rom(chip8, rom);

    // clear the screen state
    memset(chip8->video, 0, 64 * 32);

    // clear the keyboard state
    memset(chip8->key, 0, 16);

    // set the fontset
    memcpy(chip8->memory + MEMORY_FONTSET_START, fontset, FONTSET_SIZE);
}

/**
 * Initialize the instruction set.
 * Each instruction is associated to an entry of the CHIP8-ISA array.
 * @param chip8 is a pointer to the CHIP8 struct
 */
void load_ISA(CHIP8 *chip8) {
    define_instruction(chip8, 0, 0xF000, 0x0000, &sys);
    define_instruction(chip8, 1, 0xFFFF, 0x00E0, &clear_screen);
    define_instruction(chip8, 2, 0xFFFF, 0x00EE, &ret);
    define_instruction(chip8, 3, 0xF000, 0x1000, &jump_immediate);
    define_instruction(chip8, 4, 0xF000, 0x2000, &call);
    define_instruction(chip8, 5, 0xF000, 0x3000, &skip_equal_imm);
    define_instruction(chip8, 6, 0xF000, 0x4000, &skip_not_equal_imm);
    define_instruction(chip8, 7, 0xF00F, 0x5000, &skip_equal_reg);
    define_instruction(chip8, 8, 0xF000, 0x6000, &load_immediate);
    define_instruction(chip8, 9, 0xF000, 0x7000, &add_immediate);
    define_instruction(chip8, 10, 0xF00F, 0x8000, &mov);
    define_instruction(chip8, 11, 0xF00F, 0x8001, &or);
    define_instruction(chip8, 12, 0xF00F, 0x8002, &and);
    define_instruction(chip8, 13, 0xF00F, 0x8003, &xor);
    define_instruction(chip8, 14, 0xF00F, 0x8004, &add_reg);
    define_instruction(chip8, 15, 0xF00F, 0x8005, &sub);
    define_instruction(chip8, 16, 0xF00F, 0x8006, &shift_right);
    define_instruction(chip8, 17, 0xF00F, 0x8007, &subn);
    define_instruction(chip8, 18, 0xF00F, 0x800E, &shift_left);
    define_instruction(chip8, 19, 0xF00F, 0x9000, &skip_ne_reg);
    define_instruction(chip8, 20, 0xF000, 0xA000, &load_index);
    define_instruction(chip8, 21, 0xF000, 0xB000, &jump_addr);
    define_instruction(chip8, 22, 0xF000, 0xC000, &rnd);
    define_instruction(chip8, 23, 0xF000, 0xD000, &draw);
    define_instruction(chip8, 24, 0xF0FF, 0xE09E, &skip_if_pressed);
    define_instruction(chip8, 25, 0xF0FF, 0xE0A1, &skip_if_not_pressed);
    define_instruction(chip8, 26, 0xF0FF, 0xF007, &load_timer_value);
    define_instruction(chip8, 27, 0xF0FF, 0xF00A, &load_key);
    define_instruction(chip8, 28, 0xF0FF, 0xF015, &set_timer_value);
    define_instruction(chip8, 29, 0xF0FF, 0xF018, &set_sound_value);
    define_instruction(chip8, 30, 0xF0FF, 0xF01E, &add_to_index);
    define_instruction(chip8, 31, 0xF0FF, 0xF029, &load_sprite_location);
    define_instruction(chip8, 32, 0xF0FF, 0xF033, &load_bcd_representation);
    define_instruction(chip8, 33, 0xF0FF, 0xF055, &store_registers);
    define_instruction(chip8, 34, 0xF0FF, 0xF065, &load_registers);
}

void load_rom(CHIP8 *chip8, char *path) {
    FILE *rom = fopen(path, "rb");

    if (!rom) {
        fprintf(stderr, "Unable to read the provided rom! Aborting...\n");
        exit(1);
    }

    uint16_t i = 0;
    while (!feof(rom)) {
        fread(chip8->memory + MEMORY_PGM_START + (i++), 1, sizeof(uint8_t), rom);
    }

    fclose(rom);
}

void CHIP8_set_refresh_function(CHIP8 *chip8, void (*refresh)(const uint8_t *)) {
    chip8->refresh_screen = refresh;
}

void CHIP8_set_beep_function(CHIP8 *chip8, void (*beep)()) {
    chip8->beep = beep;
}

void CHIP8_set_keyboard_input_function(CHIP8 *chip8, void (*keyboard_input)(uint8_t *keyboard)) {
    chip8->keyboard_input = keyboard_input;
}

_Noreturn void CHIP8_loop(CHIP8 *chip8) {
    while (1) {
        // fetch instruction
        uint16_t opcode = (chip8->memory[chip8->PC] << 8) | (chip8->memory[chip8->PC + 1]);

        chip8->PC = chip8->PC + 2;
        for (int i = 0; i < ISA_SIZE; i++) {
            if ((opcode & chip8->ISA[i].mask) == chip8->ISA[i].opcode) {
                chip8->ISA[i].execute(chip8, opcode);
            }
        }

        if (chip8->draw_flag) {
            chip8->refresh_screen(chip8->video);
            chip8->draw_flag = 0;
        }

        // update timers
        if (chip8->delay_timer) {
            chip8->delay_timer--;
        }

        if (chip8->sound_timer) {
            chip8->sound_timer--;
            if (!chip8->sound_timer) {
                chip8->beep();
            }
        }

        // keyboard management
        chip8->keyboard_input(chip8->key);

        usleep(CLK_PERIOD);
    }
}


/**
 * Helper function used to CHIP8_init instruction_s structs.
 *
 * @param chip8 is a pointer to the emulator
 * @param index is the index of the instruction inside the
 *        CHIP8.ISA array
 * @param mask is a bitmask used to define which bits
 *        of an instruction are part of the opcode and which
 *        others are options or immediate values
 * @param opcode is the instruction's opcode
 * @param execute is the instruction's action
 */
void define_instruction(CHIP8 *chip8, int index,
                        uint16_t mask, uint16_t opcode,
                        void (*execute)(CHIP8 *, uint16_t)) {
    chip8->ISA[index].mask = mask;
    chip8->ISA[index].opcode = opcode;
    chip8->ISA[index].execute = execute;
}
