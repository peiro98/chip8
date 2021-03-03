#include <string.h>
#include <stdlib.h>
#include "CHIP-8.h"
#include "instructions.h"

void sys(CHIP8 *chip8, uint16_t opcode) {}

void clear_screen(CHIP8 *chip8, uint16_t opcode) {
    memset(chip8->video, 0, VIDEO_SIZE);
}

void ret(CHIP8 *chip8, uint16_t opcode) {
    chip8->PC = chip8->stack[--chip8->SP];
}

void jump_immediate(CHIP8 *chip8, uint16_t opcode) {
    chip8->PC = opcode & 0x0FFF;
}

void call(CHIP8 *chip8, uint16_t opcode) {
    chip8->stack[chip8->SP++] = chip8->PC;
    chip8->PC = opcode & 0x0FFF;
}

void skip_equal_imm(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t value = opcode & 0x00FF;

    if (chip8->register_file.raw[vx] == value) {
        chip8->PC = chip8->PC + 2;
    }
}

void skip_not_equal_imm(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t value = opcode & 0x00FF;

    if (chip8->register_file.raw[vx] != value) {
        chip8->PC = chip8->PC + 2;
    }
}

void skip_equal_reg(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;

    if (chip8->register_file.raw[vx] == chip8->register_file.raw[vy]) {
        chip8->PC = chip8->PC + 2;
    }
}

void load_immediate(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;
    chip8->register_file.raw[vx] = (uint8_t) opcode & 0x00FF;
}

void add_immediate(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;
    chip8->register_file.raw[vx] += (uint8_t) opcode & 0x00FF;
}

void mov(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;
    chip8->register_file.raw[vx] = chip8->register_file.raw[vy];
}

void or(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;
    chip8->register_file.raw[vx] |= chip8->register_file.raw[vy];
}

void and(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;
    chip8->register_file.raw[vx] &= chip8->register_file.raw[vy];
}

void xor(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;
    chip8->register_file.raw[vx] ^= chip8->register_file.raw[vy];
}

void add_reg(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;

    uint16_t op1 = (uint16_t) chip8->register_file.raw[vx];
    uint16_t op2 = (uint16_t) chip8->register_file.raw[vy];

    uint16_t res = op1 + op2;
    chip8->register_file.raw[vx] = (res & 0xFF);
    chip8->register_file.VF = (res & 0x100) >> 8;
}

void sub(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;

    uint8_t op1 = chip8->register_file.raw[vx];
    uint8_t op2 = chip8->register_file.raw[vy];

    chip8->register_file.raw[vx] = op1 - op2;
    chip8->register_file.VF = op1 > op2 ? 1 : 0;
}

void shift_right(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;
    chip8->register_file.VF = chip8->register_file.raw[vx] & 1;
    chip8->register_file.raw[vx] = chip8->register_file.raw[vx] >> 1;
}

void subn(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;
    uint8_t op1 = chip8->register_file.raw[vy];
    uint8_t op2 = chip8->register_file.raw[vx];

    uint16_t res = op1 - op2;
    chip8->register_file.raw[vx] = (res & 0xFF);
    chip8->register_file.VF = op1 > op2 ? 1 : 0;
}

void shift_left(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;
    chip8->register_file.VF = (chip8->register_file.raw[vx] & 0x80) >> 7;
    chip8->register_file.raw[vx] = chip8->register_file.raw[vx] << 1;
}

void skip_ne_reg(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;

    if (chip8->register_file.raw[vx] != chip8->register_file.raw[vy]) {
        chip8->PC = chip8->PC + 2;
    }
}

void load_index(CHIP8 *chip8, uint16_t opcode) {
    chip8->I = opcode & 0x0FFF;
}

void jump_addr(CHIP8 *chip8, uint16_t opcode) {
    chip8->PC = chip8->register_file.V0 + (opcode & 0x0FFF);
}

void rnd(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;

    chip8->register_file.raw[vx] = (rand() % 255) & (opcode & 0x00FF);
}

void draw(CHIP8 *chip8, uint16_t opcode) {
    uint8_t bx = chip8->register_file.raw[(opcode & 0x0F00) >> 8];
    uint8_t by = chip8->register_file.raw[(opcode & 0x00F0) >> 4];
    uint8_t height = opcode & 0x000F;

    chip8->register_file.VF = 0;
    for (uint8_t yi = 0; yi < height; yi++) {
        uint8_t p = chip8->memory[chip8->I + yi];

        for (uint8_t xi = 0; xi < 8; xi++) {
            if (p & (0x80 >> xi)) {
                //uint16_t pos = (((bx + xi) % 64) + (((by + yi) % 32) * 64));
                uint16_t pos = ((bx + xi) + ((by + yi) * 64)) % (64 * 32);
                uint8_t prev = chip8->video[pos];
                chip8->video[pos] ^= 1;
                chip8->register_file.VF |= (prev && !chip8->video[pos]);
            }
        }
    }

    chip8->draw_flag = 1;
}

void skip_if_pressed(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;

    if (chip8->key[chip8->register_file.raw[vx]]) {
        chip8->PC = chip8->PC + 2;
    }
}

void skip_if_not_pressed(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;

    if (!chip8->key[chip8->register_file.raw[vx]]) {
        chip8->PC = chip8->PC + 2;
    }
}

void load_timer_value(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;

    chip8->register_file.raw[vx] = chip8->delay_timer;
}

void load_key(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;
    int pressed = 0;

    for (int i = 0; i < NUM_KEYS; i++) {
        if (chip8->key[i]) {
            chip8->register_file.raw[vx] = chip8->key[i];
            pressed++;
        }
    }

    if (!pressed) {
        chip8->PC = chip8->PC - 2;
    }
}

void set_timer_value(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;

    chip8->delay_timer = chip8->register_file.raw[vx];
}

void set_sound_value(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;

    chip8->sound_timer = chip8->register_file.raw[vx];
}

void add_to_index(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;

    chip8->I = chip8->I + chip8->register_file.raw[vx];
}

void load_sprite_location(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;

    chip8->I = MEMORY_FONTSET_START + 5 * chip8->register_file.raw[vx];
}

void load_bcd_representation(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t value = chip8->register_file.raw[vx];

    chip8->memory[chip8->I + 0] = value / 100;
    chip8->memory[chip8->I + 1] = (value % 100) / 10;
    chip8->memory[chip8->I + 2] = value % 10;
}

void store_registers(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;

    memcpy(&chip8->memory[chip8->I], &chip8->register_file.raw, vx + 1);
}

void load_registers(CHIP8 *chip8, uint16_t opcode) {
    uint8_t vx = (opcode & 0x0F00) >> 8;

    memcpy(&chip8->register_file.raw, &chip8->memory[chip8->I], vx + 1);
}