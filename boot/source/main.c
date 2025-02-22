//main.c

/*
AUTHORS: Edwin Duong, Mark Lysacks
*/

#include <stdlib.h>
#include <wiiuse/wpad.h>
#include <grrlib.h>

#include "BMfont1_png.h"
#include "BMfont2_png.h"
#include "BMfont3_png.h"
#include "BMfont4_png.h"
#include "BMfont5_png.h"
#include "title_jpg_jpg.h"
#include "goblin_1_jpg.h"
#include "goblin_2_jpg.h"
#include "goblin_3_jpg.h"
#include "goblin_4_jpg.h"
#include "table_1_png.h"
#include "table_2_png.h"
#include "table_3_png.h"
#include "table_4_png.h"

// RGBA Colors
#define GRRLIB_BLACK   0x000000FF
#define GRRLIB_MAROON  0x800000FF
#define GRRLIB_GREEN   0x008000FF
#define GRRLIB_OLIVE   0x808000FF
#define GRRLIB_NAVY    0x000080FF
#define GRRLIB_PURPLE  0x800080FF
#define GRRLIB_TEAL    0x008080FF
#define GRRLIB_GRAY    0x808080FF
#define GRRLIB_SILVER  0xC0C0C0FF
#define GRRLIB_RED     0xFF0000FF
#define GRRLIB_LIME    0x00FF00FF
#define GRRLIB_YELLOW  0xFFFF00FF
#define GRRLIB_BLUE    0x0000FFFF
#define GRRLIB_FUCHSIA 0xFF00FFFF
#define GRRLIB_AQUA    0x00FFFFFF
#define GRRLIB_WHITE   0xFFFFFFFF

// Thresholds
#define TOP 12200
#define BOT 4200

#define NUM_PLAYERS 2

typedef struct Goblin {
    int glizzies;
    int topped;
    int bottomed;
} Goblin;

int gaming = 0;
s32 left = 0, top = 0, right = 640, bottom = 480;

Goblin goblin[NUM_PLAYERS];

GRRLIB_texImg *tex_BMfont1;
GRRLIB_texImg *tex_BMfont2;
GRRLIB_texImg *tex_BMfont3;
GRRLIB_texImg *tex_BMfont4;
GRRLIB_texImg *tex_BMfont5;
GRRLIB_texImg *tex_title;
GRRLIB_texImg *tex_goblin_1;
GRRLIB_texImg *tex_goblin_2;
GRRLIB_texImg *tex_goblin_3;
GRRLIB_texImg *tex_goblin_4;
GRRLIB_texImg *tex_table_1;
GRRLIB_texImg *tex_table_2;
GRRLIB_texImg *tex_table_3;
GRRLIB_texImg *tex_table_4;

void title() {
    const u32 wpadheld = WPAD_ButtonsHeld(0);

    GRRLIB_DrawImg((640 - 480) / 2, top, tex_title, 0, 2, 1.9, GRRLIB_WHITE);
    GRRLIB_Printf((640 - 368) / 2, 480 - 16 - 4, tex_BMfont2, GRRLIB_GREEN, 1, "PRESS A AND B TO START");
    // 23 chars

    if((wpadheld & WPAD_BUTTON_A) && (wpadheld & WPAD_BUTTON_B)) {
        WPAD_SetMotionPlus(-1, 1);
        gaming = 1;
    }
}

void game() {
    for(int i = 0; i < NUM_PLAYERS; i++) {
        WPADData *wd = WPAD_Data(i);
        short p = wd->exp.mp.rx;

        GRRLIB_texImg *tex_goblin = tex_goblin_4;
        if(goblin[i].bottomed && p < BOT) tex_goblin = tex_goblin_1;
        if(goblin[i].bottomed && p > BOT) tex_goblin = tex_goblin_2;
        if(goblin[i].topped) tex_goblin = tex_goblin_3;

        GRRLIB_texImg *tex_table = tex_table_1;
        if(goblin[i].glizzies < 20) {
            tex_table = tex_table_4;
        } else if (goblin[i].glizzies < 40) {
            tex_table = tex_table_3;
        } else if (goblin[i].glizzies < 60) {
            tex_table = tex_table_2;
        }

        GRRLIB_DrawImg(left + (right / (i * NUM_PLAYERS)), top, tex_goblin, 0, 1, 1, GRRLIB_WHITE);
        GRRLIB_DrawImg(left + (right / (i * NUM_PLAYERS)), top, tex_table, 0, 1, 1, GRRLIB_WHITE);

        if(p < BOT) goblin[i].bottomed = 1;
        if(goblin[i].bottomed == 1 && p > TOP) goblin[i].topped = 1;
        if(goblin[i].topped == 1 && goblin[i].bottomed == 1 && p > BOT && p < TOP) {
            goblin[i].bottomed = 0;
            goblin[i].topped = 0;
            goblin[i].glizzies++;
        }
    }
}

void load_textures();

void free_textures();

int main() {
    GRRLIB_Init();

    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
    WPAD_SetDataFormat(WPAD_CHAN_1, WPAD_FMT_BTNS_ACC_IR);

    load_textures();

    while(1) {
        WPAD_SetVRes(0, 640, 480);
        WPAD_ScanPads();
        const u32 wpaddown = WPAD_ButtonsDown(0);

        GRRLIB_FillScreen(GRRLIB_WHITE);
        WPAD_Rumble(WPAD_CHAN_0, 0);
        WPAD_Rumble(WPAD_CHAN_1, 0);

        if(gaming)
            game();
        else
            title();

        if(wpaddown & WPAD_BUTTON_HOME) {
            break;
        }

        GRRLIB_Render();
    }

    free_textures();

    GRRLIB_Exit();

    return 0;
}

void load_textures() {
    tex_BMfont1 = GRRLIB_LoadTexture(BMfont1_png);
    GRRLIB_InitTileSet(tex_BMfont1, 32, 32, 32);

    tex_BMfont2 = GRRLIB_LoadTexture(BMfont2_png);
    GRRLIB_InitTileSet(tex_BMfont2, 16, 16, 32);

    tex_BMfont3 = GRRLIB_LoadTexture(BMfont3_png);
    GRRLIB_InitTileSet(tex_BMfont3, 32, 32, 32);

    tex_BMfont4 = GRRLIB_LoadTexture(BMfont4_png);
    GRRLIB_InitTileSet(tex_BMfont4, 16, 16, 32);

    tex_BMfont5 = GRRLIB_LoadTexture(BMfont5_png);
    GRRLIB_InitTileSet(tex_BMfont5, 8, 16, 0);

    tex_title = GRRLIB_LoadTexture(title_jpg_jpg);
    tex_goblin_1 = GRRLIB_LoadTexture(goblin_1_jpg);
    tex_goblin_2 = GRRLIB_LoadTexture(goblin_2_jpg);
    tex_goblin_3 = GRRLIB_LoadTexture(goblin_3_jpg);
    tex_goblin_4 = GRRLIB_LoadTexture(goblin_4_jpg);
    tex_table_1 = GRRLIB_LoadTexture(table_1_png);
    tex_table_2 = GRRLIB_LoadTexture(table_2_png);
    tex_table_3 = GRRLIB_LoadTexture(table_3_png);
    tex_table_4 = GRRLIB_LoadTexture(table_4_png);
}

void free_textures() {
    GRRLIB_FreeTexture(tex_title);
    GRRLIB_FreeTexture(tex_BMfont1);
    GRRLIB_FreeTexture(tex_BMfont2);
    GRRLIB_FreeTexture(tex_BMfont3);
    GRRLIB_FreeTexture(tex_BMfont4);
    GRRLIB_FreeTexture(tex_BMfont5);
}
