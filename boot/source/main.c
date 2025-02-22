// main.c
// AUTHORS: Edwin Duong, Mark Lysack

// Includes
#include <stdlib.h>
#include <wiiuse/wpad.h>
#include <grrlib.h>
#include <asndlib.h>
#include <mp3player.h>
#include <ogc/lwp_watchdog.h>

// Images
#include "BMfont1_png.h"
#include "BMfont2_png.h"
#include "BMfont3_png.h"
#include "BMfont4_png.h"
#include "BMfont5_png.h"

#include "title_bg_jpg.h"
#include "title_png.h"

#include "game_bg_jpg.h"
#include "table_1_png.h"
#include "table_2_png.h"
#include "table_3_png.h"
#include "table_4_png.h"
#include "goblin_1_png.h"
#include "goblin_2_png.h"
#include "goblin_3_png.h"
#include "goblin_4_png.h"
#include "goblin_5_png.h"
#include "hypercam_jpg.h"

// Audios
#include "goblin_mp3.h"
#include "monkeys_mp3.h"

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

// Movement thresholds
#define TOP_THRESH 12200
#define BOT_THRESH 4200

// Gameplay settings
#define NUM_PLAYERS 2

// Draw constants
const s32 LEFT = 0;
const s32 TOP = 0;
const s32 RIGHT = 640;
const s32 BOTTOM = 480;

typedef struct Goblin {
    int glizzies;
    int topped;
    int bottomed;
    u32 finished;
} Goblin;

// Game state
int gaming = 0;
u32 start_time;
Goblin goblin[NUM_PLAYERS];

// Textures
GRRLIB_texImg *tex_BMfont1;
GRRLIB_texImg *tex_BMfont2;
GRRLIB_texImg *tex_BMfont3;
GRRLIB_texImg *tex_BMfont4;
GRRLIB_texImg *tex_BMfont5;

GRRLIB_texImg *tex_title_bg;
GRRLIB_texImg *tex_title;

GRRLIB_texImg *tex_game_bg;
GRRLIB_texImg *tex_table_1;
GRRLIB_texImg *tex_table_2;
GRRLIB_texImg *tex_table_3;
GRRLIB_texImg *tex_table_4;
GRRLIB_texImg *tex_goblin_1;
GRRLIB_texImg *tex_goblin_2;
GRRLIB_texImg *tex_goblin_3;
GRRLIB_texImg *tex_goblin_4;
GRRLIB_texImg *tex_goblin_5;
GRRLIB_texImg *tex_hypercam;

// Title animations
u32 title_zoom_time;
f32 title_zoom_max = 0.01;
int title_zoom_dir = 1;
int title_zoom_dur = 2000;

void title() {
    const u32 wpadheld = WPAD_ButtonsHeld(0);

    u32 curr_time = ticks_to_millisecs(gettime());

    if(curr_time > title_zoom_time + title_zoom_dur) {
        title_zoom_time = curr_time;
        title_zoom_dir *= -1;
    }

    f32 s = 1 - (title_zoom_max * title_zoom_dir) + 2 * title_zoom_max * title_zoom_dir * (curr_time - title_zoom_time) / title_zoom_dur;

    GRRLIB_DrawImg(LEFT, TOP, tex_title_bg, 0, 1, 1, GRRLIB_WHITE);
    GRRLIB_DrawImg((RIGHT - 320 * s) / 2, (BOTTOM - 320 * s) / 2, tex_title, 0, s, s, GRRLIB_WHITE);

    GRRLIB_Printf((RIGHT - 352) / 2, BOTTOM - 48, tex_BMfont2, GRRLIB_GREEN, 1, "PRESS A AND B TO START");

    if(!MP3Player_IsPlaying()) {
        MP3Player_PlayBuffer(goblin_mp3, goblin_mp3_size, NULL);
    }

    if((wpadheld & WPAD_BUTTON_A) && (wpadheld & WPAD_BUTTON_B)) {
        WPAD_SetMotionPlus(WPAD_CHAN_ALL, 1);
        gaming = 1;
        start_time = ticks_to_millisecs(gettime()) + 5000;
        MP3Player_Stop();
        MP3Player_PlayBuffer(monkeys_mp3, monkeys_mp3_size, NULL);
    }
}

void game() {
    const u32 wpaddown = WPAD_ButtonsDown(0);
    u32 curr_time = ticks_to_millisecs(gettime());

    GRRLIB_DrawImg(LEFT, TOP, tex_game_bg, 0, 1, 1, GRRLIB_WHITE);

    if(curr_time < start_time) {
        for(int i = 0; i < NUM_PLAYERS; i++) {
            GRRLIB_DrawImg(LEFT + (i * (RIGHT / NUM_PLAYERS)), TOP, tex_goblin_4, 0, 1, 1, GRRLIB_WHITE);
            GRRLIB_DrawImg(LEFT + (i * (RIGHT / NUM_PLAYERS)), TOP, tex_table_4, 0, 1, 1, GRRLIB_WHITE);
            goblin[i].finished = 0;
        }
        f32 s = 2 + 4 * ((start_time - curr_time) % 1000) / 1000.0;
        GRRLIB_Printf((RIGHT - 32 * s) / 2, (BOTTOM - 32 * s) / 2, tex_BMfont3, GRRLIB_LIME, s, "%d", (int)(start_time - curr_time) / 1000 + 1);
    } else {
        for(int i = 0; i < NUM_PLAYERS; i++) {
            WPADData *wd = WPAD_Data(i);
            short p = wd->exp.mp.rx;

            GRRLIB_texImg *tex_goblin = tex_goblin_4;
            if(goblin[i].glizzies < 60) {
                if(goblin[i].bottomed && p < BOT_THRESH) tex_goblin = tex_goblin_1;
                if(goblin[i].bottomed && p > BOT_THRESH) tex_goblin = tex_goblin_2;
                if(goblin[i].topped) tex_goblin = tex_goblin_3;
            } else {
                tex_goblin = tex_goblin_5;
                if(goblin[i].finished == 0) {
                    goblin[i].finished = curr_time - start_time;
                }
            }

            GRRLIB_texImg *tex_table = tex_table_1;
            if(goblin[i].glizzies < 20) {
                tex_table = tex_table_4;
            } else if (goblin[i].glizzies < 40) {
                tex_table = tex_table_3;
            } else if (goblin[i].glizzies < 60) {
                tex_table = tex_table_2;
            }

            GRRLIB_DrawImg(LEFT + (i * (RIGHT / NUM_PLAYERS)), TOP, tex_goblin, 0, 1, 1, GRRLIB_WHITE);
            GRRLIB_DrawImg(LEFT + (i * (RIGHT / NUM_PLAYERS)), TOP, tex_table, 0, 1, 1, GRRLIB_WHITE);

            if(goblin[i].glizzies < 60) {
                if(p < BOT_THRESH) goblin[i].bottomed = 1;
                if(goblin[i].bottomed == 1 && p > TOP_THRESH) goblin[i].topped = 1;
                if(goblin[i].topped == 1 && goblin[i].bottomed == 1 && p > BOT_THRESH && p < TOP_THRESH) {
                    goblin[i].bottomed = 0;
                    goblin[i].topped = 0;
                    goblin[i].glizzies++;
                }
            } else {
                goblin[i].topped = 0;
                goblin[i].bottomed = 0;
                u32 m, s, ms;
                ms = goblin[i].finished % 1000;
                s = (goblin[i].finished / 1000) % 60;
                m = ((goblin[i].finished / 1000) / 60 ) % 60;
                GRRLIB_Printf(LEFT + (i * (RIGHT / NUM_PLAYERS)) + 88, BOTTOM - 48, tex_BMfont2, GRRLIB_YELLOW, 1, "%02d:%02d:%03d", m, s, ms);
            }
        }
        u32 d, m, s, ms;
        d = curr_time - start_time;
        ms = d % 1000;
        s = (d / 1000) % 60;
        m = ((d / 1000) / 60 ) % 60;
        GRRLIB_Printf((RIGHT - 144) / 2, 64, tex_BMfont4, GRRLIB_RED, 1, "%02d:%02d:%03d", m, s, ms);
    }

    GRRLIB_DrawImg(LEFT, TOP, tex_hypercam, 0, 0.5, 0.5, GRRLIB_WHITE);

    if(!MP3Player_IsPlaying()) {
        MP3Player_PlayBuffer(monkeys_mp3, monkeys_mp3_size, NULL);
    }

    int done = 1;
    for(int i = 0; done && i < NUM_PLAYERS; i++) {
        if(goblin[i].glizzies < 60) {
            done = 0;
        }
    }

    if(done && wpaddown & WPAD_BUTTON_1)  {
        gaming = 0;
        start_time = 0;
        for(int i = 0; i < NUM_PLAYERS; i++) {
            goblin[i].topped = 0;
            goblin[i].bottomed = 0;
            goblin[i].glizzies = 0;
            goblin[i].finished = 0;
            MP3Player_Stop();
            MP3Player_PlayBuffer(goblin_mp3, goblin_mp3_size, NULL);
        }
    }
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

    tex_title_bg = GRRLIB_LoadTexture(title_bg_jpg);
    tex_title = GRRLIB_LoadTexture(title_png);

    tex_game_bg = GRRLIB_LoadTexture(game_bg_jpg);
    tex_table_1 = GRRLIB_LoadTexture(table_1_png);
    tex_table_2 = GRRLIB_LoadTexture(table_2_png);
    tex_table_3 = GRRLIB_LoadTexture(table_3_png);
    tex_table_4 = GRRLIB_LoadTexture(table_4_png);
    tex_goblin_1 = GRRLIB_LoadTexture(goblin_1_png);
    tex_goblin_2 = GRRLIB_LoadTexture(goblin_2_png);
    tex_goblin_3 = GRRLIB_LoadTexture(goblin_3_png);
    tex_goblin_4 = GRRLIB_LoadTexture(goblin_4_png);
    tex_goblin_5 = GRRLIB_LoadTexture(goblin_5_png);
    tex_hypercam = GRRLIB_LoadTexture(hypercam_jpg);
}

void free_textures() {
    GRRLIB_FreeTexture(tex_BMfont1);
    GRRLIB_FreeTexture(tex_BMfont2);
    GRRLIB_FreeTexture(tex_BMfont3);
    GRRLIB_FreeTexture(tex_BMfont4);
    GRRLIB_FreeTexture(tex_BMfont5);

    GRRLIB_FreeTexture(tex_title_bg);
    GRRLIB_FreeTexture(tex_title);

    GRRLIB_FreeTexture(tex_game_bg);
    GRRLIB_FreeTexture(tex_table_1);
    GRRLIB_FreeTexture(tex_table_2);
    GRRLIB_FreeTexture(tex_table_3);
    GRRLIB_FreeTexture(tex_table_4);
    GRRLIB_FreeTexture(tex_goblin_1);
    GRRLIB_FreeTexture(tex_goblin_2);
    GRRLIB_FreeTexture(tex_goblin_3);
    GRRLIB_FreeTexture(tex_goblin_4);
    GRRLIB_FreeTexture(tex_goblin_5);
    GRRLIB_FreeTexture(tex_hypercam);
}

void setup() {
    GRRLIB_Init();
    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
    load_textures();
	ASND_Init();
	MP3Player_Init();
}

void teardown() {
    free_textures();
    GRRLIB_Exit();
}

void draw() {
    while(1) {
        WPAD_SetVRes(0, 640, 480);
        WPAD_ScanPads();
        const u32 wpaddown = WPAD_ButtonsDown(0);

        GRRLIB_FillScreen(GRRLIB_BLACK);
        WPAD_Rumble(WPAD_CHAN_ALL, 0);

        if(gaming)
            game();
        else
            title();

        if(wpaddown & WPAD_BUTTON_HOME)
            break;

        GRRLIB_Render();
    }
}

int main() {
    setup();
    draw();
    teardown();
    return 0;
}
