/*===========================================
        GRRLIB (GX Version)
        - Example Code -

        How To use Bitmap Fonts
============================================*/
#include <grrlib.h>

#include <ogc/lwp_watchdog.h>   // Needed for gettime and ticks_to_millisecs
#include <stdlib.h>
#include <wiiuse/wpad.h>

#include "BMfont1_png.h"
#include "BMfont2_png.h"
#include "BMfont3_png.h"
#include "BMfont4_png.h"
#include "BMfont5_png.h"
#include "test_jpg_jpg.h"
#include "test_bmp_bmp.h"
#include "sprite_png.h"
#include "ocean_bmf.h"
#include "frontal_bmf.h"
#include "title_png_png.h"
#include "homepage_jpg_jpg.h"

// Tile stuff
#define TILE_DELAY  10
#define TILE_UP     12 * 0
#define TILE_RIGHT  12 * 1
#define TILE_DOWN   12 * 2
#define TILE_LEFT   12 * 3
#define TILE_UP2    12 * 4 + 9
#define TILE_RIGHT2 12 * 5 + 9
#define TILE_DOWN2  12 * 6 + 9
#define TILE_LEFT2  12 * 7 + 9

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
#define TOP 100
#define BOT 380

typedef struct Player {
    int glizzies;
    int topped_out;
    int bottomed_out;
} Player;

static u8 CalculateFrameRate(void);

s32 page = 0; // defines what is displayed in the stage
// 0 is the main menu

s32 left = 0, top = 0, frame = TILE_DOWN + 1;
u32 wait = TILE_DELAY, direction = TILE_DOWN, direction_new = TILE_DOWN;
u8 FPS = 0;

ir_t ir1;
ir_t ir2;

Player p1;
Player p2;

// real pngs
GRRLIB_texImg *tex_title_png;
GRRLIB_texImg *tex_homepage_jpg_jpg;

GRRLIB_texImg *tex_test_jpg;
GRRLIB_texImg *tex_test_bmp;
GRRLIB_bytemapFont *bmf_Font1;
GRRLIB_bytemapFont *bmf_Font2 ;
GRRLIB_texImg *tex_sprite_png;
GRRLIB_texImg *tex_BMfont1;
GRRLIB_texImg *tex_BMfont2;
GRRLIB_texImg *tex_BMfont3;
GRRLIB_texImg *tex_BMfont4;
GRRLIB_texImg *tex_BMfont5;

void main_menu() {
    // const u32 wpaddown = WPAD_ButtonsDown(0);
    const u32 wpadheld = WPAD_ButtonsHeld(0);

    // GRRLIB_DrawImg(left, top, tex_title_png, 0, 1, 1, GRRLIB_WHITE);
    // (640 - 480) / 2, puts the image in the middle
    GRRLIB_DrawImg((640 - 480) / 2, top, tex_homepage_jpg_jpg, 0, 2, 1.9, GRRLIB_WHITE);
    GRRLIB_Printf((640 - 368) / 2, 480 - 16 - 4, tex_BMfont2, GRRLIB_WHITE, 1, "PRESS A AND B TO START");
    // 23 chars

    if((wpadheld & WPAD_BUTTON_A) && (wpadheld & WPAD_BUTTON_B)) {
        page++;
    }
}

void game() {
    u32 c1 = GRRLIB_WHITE;
    u32 c2 = GRRLIB_WHITE;
    if(p1.topped_out == 1) {
        c1 = GRRLIB_GREEN;
    }
    if(p1.bottomed_out == 1) {
        c1 = GRRLIB_RED;
    }
    GRRLIB_Printf(left, top+300, tex_BMfont3, c1, 1, "IR1 Y VALUE: %d", (int)ir1.y);
    GRRLIB_Printf(left, top+400, tex_BMfont3, c2, 1, "IR2 Y VALUE: %d", (int)ir2.y);
    if(ir1.y < TOP && p1.bottomed_out == 1) p1.topped_out = 1; else p1.topped_out = 0;
    if(ir1.y > BOT) p1.bottomed_out = 1;
}

int main() {
    GRRLIB_Init();

    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
    WPAD_SetDataFormat(WPAD_CHAN_1, WPAD_FMT_BTNS_ACC_IR);

    // used pngs
    tex_title_png = GRRLIB_LoadTexture(title_png_png);

    tex_homepage_jpg_jpg = GRRLIB_LoadTexture(homepage_jpg_jpg);
    tex_test_jpg = GRRLIB_LoadTexture(test_jpg_jpg);
    tex_test_bmp = GRRLIB_LoadTexture(test_bmp_bmp);

    bmf_Font1 = GRRLIB_LoadBMF(ocean_bmf);
    bmf_Font2 = GRRLIB_LoadBMF(frontal_bmf);

    tex_sprite_png = GRRLIB_LoadTexture(sprite_png);
    GRRLIB_InitTileSet(tex_sprite_png, 24, 32, 0);

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

    while(1) {
        WPAD_SetVRes(0, 640, 480);
        WPAD_ScanPads();
        const u32 wpaddown = WPAD_ButtonsDown(0);
        const u32 wpadheld = WPAD_ButtonsHeld(0);

        WPAD_IR(WPAD_CHAN_0, &ir1);
        WPAD_IR(WPAD_CHAN_1, &ir2);

        GRRLIB_FillScreen(GRRLIB_WHITE);    // Clear the screen
        WPAD_Rumble(WPAD_CHAN_0, 0);
        WPAD_Rumble(WPAD_CHAN_1, 0);
        switch(page)
        {
            case 1:   // Draw images
                game();
                break;
            default:
                main_menu();
        }
        GRRLIB_Printf(500, 27, tex_BMfont5, GRRLIB_WHITE, 1, "Current FPS: %d", FPS);

        if(wpaddown & WPAD_BUTTON_HOME) {
            break;
        }
        if(wpadheld & WPAD_BUTTON_LEFT) {
            if(wpadheld & WPAD_BUTTON_B || page == 1)
                left -= 2;
            else
                left--;
            direction_new = TILE_LEFT;  // for tile example
        }
        if(wpadheld & WPAD_BUTTON_RIGHT) {
            if(wpadheld & WPAD_BUTTON_B || page == 1)
                left += 2;
            else
                left++;
            direction_new = TILE_RIGHT;  // for tile example
        }
        if(wpadheld & WPAD_BUTTON_UP) {
            if(wpadheld & WPAD_BUTTON_B || page == 1)
                top -= 2;
            else
                top--;
            direction_new = TILE_UP;  // for tile example
        }
        if(wpadheld & WPAD_BUTTON_DOWN) {
            if(wpadheld & WPAD_BUTTON_B || page == 1)
                top += 2;
            else
                top++;
            direction_new = TILE_DOWN;  // for tile example
        }
        if(wpadheld & WPAD_BUTTON_1 && wpadheld & WPAD_BUTTON_2) {
            WPAD_Rumble(WPAD_CHAN_0, 1); // Rumble on
            WPAD_Rumble(WPAD_CHAN_1, 1); // Rumble on
            GRRLIB_ScrShot("sd:/grrlib.png");
            WPAD_Rumble(WPAD_CHAN_0, 0); // Rumble off
            WPAD_Rumble(WPAD_CHAN_1, 0); // Rumble off
        }

        GRRLIB_Render();
        FPS = CalculateFrameRate();
    }
    // Free some textures
    GRRLIB_FreeTexture(tex_test_jpg);
    GRRLIB_FreeTexture(tex_test_bmp);
    GRRLIB_FreeTexture(tex_sprite_png);
    GRRLIB_FreeTexture(tex_BMfont1);
    GRRLIB_FreeTexture(tex_BMfont2);
    GRRLIB_FreeTexture(tex_BMfont3);
    GRRLIB_FreeTexture(tex_BMfont4);
    GRRLIB_FreeTexture(tex_BMfont5);
    GRRLIB_FreeBMF(bmf_Font1);
    GRRLIB_FreeBMF(bmf_Font2);
    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB
    return 0;
}

/**
 * This function calculates the number of frames we render each second.
 * @return The number of frames per second.
 */
static u8 CalculateFrameRate(void) {
    static u8 frameCount = 0;
    static u32 lastTime;
    static u8 FPS = 0;
    const u32 currentTime = ticks_to_millisecs(gettime());

    frameCount++;
    if(currentTime - lastTime > 1000) {
        lastTime = currentTime;
        FPS = frameCount;
        frameCount = 0;
    }
    return FPS;
}
