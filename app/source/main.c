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
#define TOP 160
#define BOT 320

typedef struct Goblin {
    int glizzies;
    int topped;
    int bottomed;
} Goblin;

s32 page = 0;
s32 left = 0, top = 0, right = 640, bottom = 480;

ir_t ir1;
ir_t ir2;

Goblin g1;
Goblin g2;

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

void title() {
    const u32 wpadheld = WPAD_ButtonsHeld(0);

    GRRLIB_DrawImg((640 - 480) / 2, top, tex_title, 0, 2, 1.9, GRRLIB_WHITE);
    GRRLIB_Printf((640 - 368) / 2, 480 - 16 - 4, tex_BMfont2, GRRLIB_GREEN, 1, "PRESS A AND B TO START");
    // 23 chars

    if((wpadheld & WPAD_BUTTON_A) && (wpadheld & WPAD_BUTTON_B)) {
        page++;
    }
}

void game() {
    GRRLIB_texImg *i1 = tex_goblin_4;
    GRRLIB_texImg *i2 = tex_goblin_4;

    if(g1.bottomed && ir1.y > BOT) i1 = tex_goblin_1;
    if(g1.bottomed && ir1.y < BOT) i1 = tex_goblin_2;
    if(g1.topped) i1 = tex_goblin_3;

    if(g2.bottomed && ir2.y > BOT) i2 = tex_goblin_1;
    if(g2.bottomed && ir2.y < BOT) i2 = tex_goblin_2;
    if(g2.topped) i2 = tex_goblin_3;

    GRRLIB_DrawImg(left, top, i1, 0, 1, 1, GRRLIB_WHITE);
    GRRLIB_DrawImg(left + right / 2, top, i2, 0, 1, 1, GRRLIB_WHITE);

    if(ir1.y > BOT) g1.bottomed = 1;
    if(g1.bottomed == 1 && ir1.y < TOP) g1.topped = 1;
    if(g1.topped == 1 && g1.bottomed == 1 && ir1.y < BOT && ir1.y > TOP) {
        g1.bottomed = 0;
        g1.topped = 0;
        g1.glizzies++;
    }
    
    if(ir2.y > BOT) g2.bottomed = 1;
    if(g2.bottomed == 1 && ir2.y < TOP) g2.topped = 1;
    if(g2.topped == 1 && g2.bottomed == 1 && ir2.y < BOT && ir2.y > TOP) {
        g2.bottomed = 0;
        g2.topped = 0;
        g2.glizzies++;
    }
}

int main() {
    GRRLIB_Init();

    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
    WPAD_SetDataFormat(WPAD_CHAN_1, WPAD_FMT_BTNS_ACC_IR);

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

    while(1) {
        WPAD_SetVRes(0, 640, 480);
        WPAD_ScanPads();
        const u32 wpaddown = WPAD_ButtonsDown(0);

        WPAD_IR(WPAD_CHAN_0, &ir1);
        WPAD_IR(WPAD_CHAN_1, &ir2);

        GRRLIB_FillScreen(GRRLIB_WHITE);
        WPAD_Rumble(WPAD_CHAN_0, 0);
        WPAD_Rumble(WPAD_CHAN_1, 0);
        switch(page)
        {
            case 1:
                game();
                break;
            default:
                title();
        }

        if(wpaddown & WPAD_BUTTON_HOME) {
            break;
        }

        GRRLIB_Render();
    }

    // Free textures
    GRRLIB_FreeTexture(tex_title);
    GRRLIB_FreeTexture(tex_BMfont1);
    GRRLIB_FreeTexture(tex_BMfont2);
    GRRLIB_FreeTexture(tex_BMfont3);
    GRRLIB_FreeTexture(tex_BMfont4);
    GRRLIB_FreeTexture(tex_BMfont5);
    GRRLIB_Exit();

    return 0;
}
