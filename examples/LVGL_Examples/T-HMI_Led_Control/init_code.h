#ifndef _INIT_CODE_H_
#define _INIT_CODE_H_

#include <stdint.h>

/**
 * The LCD needs a bunch of command/argument values to be initialized.
 * They are stored in this struct.
 */
typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t len;
    uint16_t delay;
} lcd_init_cmd_t;


static const lcd_init_cmd_t st_init_cmds[] = {
    {
        .cmd = 0x01,
        .data= {},
        .len = 0,
        .delay = 100
    }, {
        .cmd = 0x11,
        .data = {},
        .len = 0,
        .delay = 0
    }, {
        .cmd = 0x36,
        .data = {0x00},
        .len = 1,
        .delay = 0
    }, {
        .cmd = 0x3a,
        .data = {0x05},
        .len = 1,
        .delay = 0
    }, {
        .cmd = 0xb2,
        .data = {0x0c, 0x0c, 0x00, 0x33, 0x33},
        .len = 5,
        .delay = 0
    }, {
        .cmd = 0xb7,
        .data = {0x35},
        .len = 1,
        .delay = 0
    }, {
        .cmd = 0xbb,
        .data = {0x28},
        .len = 1,
        .delay = 0
    }, {
        .cmd = 0xc0,
        .data = {0x2c},
        .len = 1,
        .delay = 0
    }, {
        .cmd = 0xc2,
        .data = {0x01},
        .len = 1,
        .delay = 0
    }, {
        .cmd = 0xc3,
        .data = {0x0b},
        .len = 1,
        .delay = 0
    }, {
        .cmd = 0xc4,
        .data = {0x20},
        .len = 1,
        .delay = 0
    }, {
        .cmd = 0xc6,
        .data = {0x0f},
        .len = 1,
        .delay = 0
    }, {
        .cmd = 0xd0,
        .data = {0xa4, 0xa1},
        .len = 2,
        .delay = 0
    }, {
        .cmd = 0xe0,
        .data = {0xd0, 0x01, 0x08, 0x0f, 0x11, 0x2a, 0x36, 0x55, 0x44, 0x3a, 0x0b, 0x06, 0x11, 0x20},
        .len = 16,
        .delay = 0
    }, {
        .cmd = 0xe1,
        .data = {0xd0, 0x02, 0x07, 0x0a, 0x0b, 0x18, 0x34, 0x43, 0x4a, 0x2b, 0x1b, 0x1c, 0x22, 0x1f},
        .len = 16,
        .delay = 0
    }, {
        .cmd = 0x29,
        .data = {},
        .len = 0,
        .delay = 0
    }, {
        .cmd = 0x2c,
        .data = {},
        .len = 0,
        .delay = 0
    }
};

#endif
