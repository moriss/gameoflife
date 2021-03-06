/**********************************
 * Project:         Game of Life
 * Filename:        gol_render.c
 * Description:     A megjelenitessel kapcsolatos fuggvenyek es definiciok
 *
 * Copyright Telek Istvan 2015
 *
 **********************************/
#include <stdio.h>
#include <time.h>
#include "gol_main.h"
#include "gol_grid.h"
#include "gol_font.h"

static const char CELL_TEXTURE_ALIVE_PATH[] = "assets/texture/cell_rect.png";
// static const char CELL_TEXTURE_ALIVE_PATH[] = "assets/texture/cell_blue.png";
// static const char CELL_TEXTURE_ALIVE_PATH[] = "assets/texture/jelly.png";
static SDL_Texture *cell_texture_alive = NULL;

static const char CELL_TEXTURE_DEAD_PATH[] = "assets/texture/cell_rect_dead.png";
// static const char CELL_TEXTURE_DEAD_PATH[] = "assets/texture/cell_blue_dead.png";
// static const char CELL_TEXTURE_DEAD_PATH[] = "assets/texture/jelly_dead.png";
static SDL_Texture *cell_texture_dead = NULL;

static const char MENU_BG_TEXTURE_PATH[] = "assets/texture/menualpha.png";
static SDL_Texture *menubg = NULL;

static const char GAME_LOGO_TEXTURE_PATH[4][64] = {
    "assets/texture/logo01.png",
    "assets/texture/logo02.png",
    "assets/texture/logo03.png",
    "assets/texture/logo04.png",
};
static SDL_Texture *gamelogo[4] = {NULL};
static const int LOGO_ANIM_SPEED = 512;
static int logo_state = 0;

static char buf[128];

void render_init(GameVars *game_vars) {
    cell_texture_alive = Game_Load_Texture(CELL_TEXTURE_ALIVE_PATH, game_vars->renderer);
    cell_texture_dead = Game_Load_Texture(CELL_TEXTURE_DEAD_PATH, game_vars->renderer);
    menubg = Game_Load_Texture(MENU_BG_TEXTURE_PATH, game_vars->renderer);
    int i;
    for (i=0; i<4; i++) {
        gamelogo[i] = Game_Load_Texture(GAME_LOGO_TEXTURE_PATH[i], game_vars->renderer);
    }
}

void render_destroy(void) {
    if (cell_texture_alive != NULL) {
        SDL_DestroyTexture(cell_texture_alive);
        cell_texture_alive = NULL;
    }
    if (cell_texture_dead != NULL) {
        SDL_DestroyTexture(cell_texture_dead);
        cell_texture_dead = NULL;
    }
    if (menubg != NULL) {
        SDL_DestroyTexture(menubg);
        menubg = NULL;
    }
    int i;
    for (i=0; i<4; i++) {
        if (gamelogo[i] != NULL) {
            SDL_DestroyTexture(gamelogo[i]);
            gamelogo[i] = NULL;
        }
    }
}

void render_grid(GameVars *game_vars) {
    int i,j;
    SDL_Rect itemrect;
    // Az elso ervenyes elem az 1. indexu - a szegely miatt
    for (i=0; i<game_vars->grid->size_x-1; i++) {
        for (j=0; j<game_vars->grid->size_y-1; j++) {
            Cell tmpcell = game_vars->grid->cells[i+1][j+1];
            if (tmpcell.state) {
                itemrect = (SDL_Rect) {
                    game_vars->settings.cell_size *i, game_vars->settings.cell_size *j, game_vars->settings.cell_size, game_vars->settings.cell_size
                };
                SDL_RenderCopy(game_vars->renderer, cell_texture_alive, NULL, &itemrect);
            } else if (tmpcell.was_alive) {
                itemrect = (SDL_Rect) {
                    game_vars->settings.cell_size *i, game_vars->settings.cell_size *j, game_vars->settings.cell_size, game_vars->settings.cell_size
                };
                SDL_RenderCopy(game_vars->renderer, cell_texture_dead, NULL, &itemrect);
            }
        }
    }
}

void render_menu_main(GameVars *game_vars) {
    SDL_SetRenderDrawColor(game_vars->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(game_vars->renderer);
    Game_SetFontText(game_vars->game_font, "ESC: Kilépés, SPACE: Új élettér, LE: Élettér betöltése");
    Game_RenderFont(game_vars->game_font, game_vars->window_size.w/2-game_vars->game_font->texture_size.x/2, game_vars->window_size.h-game_vars->game_font->texture_size.y-8);
    int w, h;
    SDL_QueryTexture(gamelogo[logo_state/LOGO_ANIM_SPEED], NULL, NULL, &w, &h);
    SDL_Rect itemrect = (SDL_Rect) {
        game_vars->window_size.w/2-w/2, game_vars->window_size.h/2-h/2, w, h
    };
    SDL_RenderCopy(game_vars->renderer, gamelogo[logo_state/LOGO_ANIM_SPEED], NULL, &itemrect);
    if (++logo_state/LOGO_ANIM_SPEED == 4) {
        logo_state = 0;
    }
    SDL_RenderPresent(game_vars->renderer);
}

void render_menu_load(GameVars *game_vars) {
    SDL_SetRenderDrawColor(game_vars->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(game_vars->renderer);
    Game_SetFontText(game_vars->game_font, "Élettér betöltése");
    Game_RenderFont(game_vars->game_font, game_vars->window_size.w/2-game_vars->game_font->texture_size.x/2, 8);
    struct tm *save_date = localtime(&(game_vars->settings.save_modified));
    sprintf(buf, "A mentés létrehozva: %4d.%02d.%02d. %02d:%02d", 1900+save_date->tm_year, save_date->tm_mon, 1+save_date->tm_mday, save_date->tm_hour, save_date->tm_min);
    Game_SetFontText(game_vars->game_font, buf);
    Game_RenderFont(game_vars->game_font, game_vars->window_size.w/2-game_vars->game_font->texture_size.x/2, game_vars->window_size.h/2-game_vars->game_font->texture_size.y/2);
    Game_SetFontText(game_vars->game_font, "ESC: Főmenü, SPACE: Élettér betöltése");
    Game_RenderFont(game_vars->game_font, game_vars->window_size.w/2-game_vars->game_font->texture_size.x/2, game_vars->window_size.h-game_vars->game_font->texture_size.y-8);
    SDL_RenderPresent(game_vars->renderer);
}

void render_menu_new(GameVars *game_vars) {
    SDL_SetRenderDrawColor(game_vars->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(game_vars->renderer);
    Game_SetFontText(game_vars->game_font, "Új élettér");
    Game_RenderFont(game_vars->game_font, game_vars->window_size.w/2-game_vars->game_font->texture_size.x/2, 8);
    Game_SetFontText(game_vars->game_font, "ESC: Főmenü, SPACE: Szimuláció indítása");
    Game_RenderFont(game_vars->game_font, game_vars->window_size.w/2-game_vars->game_font->texture_size.x/2, game_vars->window_size.h-game_vars->game_font->texture_size.y-8);
    Game_SetFontText(game_vars->game_font, "BAL/JOBB: Grid szélessége, FEL/LE: Grid magassága");
    Game_RenderFont(game_vars->game_font, game_vars->window_size.w/2-game_vars->game_font->texture_size.x/2, game_vars->window_size.h-game_vars->game_font->texture_size.y-32);
    sprintf(buf, "Szélesség: %g", game_vars->grid_size.x);
    Game_SetFontText(game_vars->game_font, buf);
    Game_RenderFont(game_vars->game_font, game_vars->window_size.w/2-game_vars->game_font->texture_size.x/2, game_vars->window_size.h/2-game_vars->game_font->texture_size.y/2);
    sprintf(buf, "Magasság: %g",  game_vars->grid_size.y);
    Game_SetFontText(game_vars->game_font, buf);
    Game_RenderFont(game_vars->game_font, game_vars->window_size.w/2-game_vars->game_font->texture_size.x/2, game_vars->window_size.h/2+game_vars->game_font->texture_size.y/2);
    SDL_RenderPresent(game_vars->renderer);
}

void render_menu_sim_menu(GameVars *game_vars) {
    SDL_SetRenderDrawColor(game_vars->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(game_vars->renderer);
    render_grid(game_vars);
    SDL_Rect itemrect = {0, 0, game_vars->window_size.w, game_vars->window_size.h};
    SDL_RenderCopy(game_vars->renderer, menubg, NULL, &itemrect);
    Game_SetFontText(game_vars->game_font, "Szimuláció menü");
    Game_RenderFont(game_vars->game_font, game_vars->window_size.w/2-game_vars->game_font->texture_size.x/2, 8);
    Game_SetFontText(game_vars->game_font, "Q: Kilépés a főmenübe, ESC: Vissza a szimulációhoz");
    Game_RenderFont(game_vars->game_font, game_vars->window_size.w/2-game_vars->game_font->texture_size.x/2, game_vars->window_size.h-game_vars->game_font->texture_size.y-8);
    Game_SetFontText(game_vars->game_font, "LE: Szimuláció mentése, FEL: Szimuláció beállításai");
    Game_RenderFont(game_vars->game_font, game_vars->window_size.w/2-game_vars->game_font->texture_size.x/2, game_vars->window_size.h-game_vars->game_font->texture_size.y-32);
    SDL_RenderPresent(game_vars->renderer);
}

void render_menu_settings(GameVars *game_vars) {
    SDL_SetRenderDrawColor(game_vars->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(game_vars->renderer);
    Game_SetFontText(game_vars->game_font, "Szimuláció beállításai");
    Game_RenderFont(game_vars->game_font, game_vars->window_size.w/2-game_vars->game_font->texture_size.x/2, 8);
    Game_SetFontText(game_vars->game_font, "ESC: Vissza a Szimuláció menübe");
    Game_RenderFont(game_vars->game_font, game_vars->window_size.w/2-game_vars->game_font->texture_size.x/2, game_vars->window_size.h-game_vars->game_font->texture_size.y-8);
    SDL_RenderPresent(game_vars->renderer);
}

void render_menu_save(GameVars *game_vars) {
    SDL_SetRenderDrawColor(game_vars->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(game_vars->renderer);
    Game_SetFontText(game_vars->game_font, "Szimuláció mentése");
    Game_RenderFont(game_vars->game_font, game_vars->window_size.w/2-game_vars->game_font->texture_size.x/2, 8);
    Game_SetFontText(game_vars->game_font, "ESC: Vissza a Szimuláció menübe, SPACE: Szimuláció mentése");
    Game_RenderFont(game_vars->game_font, game_vars->window_size.w/2-game_vars->game_font->texture_size.x/2, game_vars->window_size.h-game_vars->game_font->texture_size.y-8);
    SDL_RenderPresent(game_vars->renderer);
}

void render_menu_save_conf(GameVars *game_vars) {
    SDL_SetRenderDrawColor(game_vars->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(game_vars->renderer);
    Game_SetFontText(game_vars->game_font, "Szimuláció mentése");
    Game_RenderFont(game_vars->game_font, game_vars->window_size.w/2-game_vars->game_font->texture_size.x/2, 8);
    Game_SetFontText(game_vars->game_font, "ESC: Vissza a Szimuláció menübe");
    Game_RenderFont(game_vars->game_font, game_vars->window_size.w/2-game_vars->game_font->texture_size.x/2, game_vars->window_size.h-game_vars->game_font->texture_size.y-8);
    Game_SetFontText(game_vars->game_font, "A szimuláció állapota elmentve.");
    Game_RenderFont(game_vars->game_font, game_vars->window_size.w/2-game_vars->game_font->texture_size.x/2, game_vars->window_size.h/2-game_vars->game_font->texture_size.y/2);
    SDL_RenderPresent(game_vars->renderer);
}

void render_sim_paused(GameVars *game_vars) {
    SDL_SetRenderDrawColor(game_vars->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(game_vars->renderer);
    render_grid(game_vars);
    Game_SetFontText(game_vars->game_font, "Szimuláció (szünetel)");
    Game_RenderFont(game_vars->game_font, game_vars->window_size.w/2-game_vars->game_font->texture_size.x/2, 8);
    Game_SetFontText(game_vars->game_font, "ESC: Szimuláció menü, SPACE: Szimuláció futtatása");
    Game_RenderFont(game_vars->game_font, game_vars->window_size.w/2-game_vars->game_font->texture_size.x/2, game_vars->window_size.h-game_vars->game_font->texture_size.y-8);
    Game_SetFontText(game_vars->game_font, "Bal egér: Cella felélesztése, Jobb egér: Cella megölése");
    Game_RenderFont(game_vars->game_font, game_vars->window_size.w/2-game_vars->game_font->texture_size.x/2, game_vars->window_size.h-game_vars->game_font->texture_size.y-32);
    SDL_RenderPresent(game_vars->renderer);

}

void render_sim_running(GameVars *game_vars) {
    SDL_SetRenderDrawColor(game_vars->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(game_vars->renderer);
    render_grid(game_vars);
    Game_SetFontText(game_vars->game_font, "Szimuláció (fut)");
    Game_RenderFont(game_vars->game_font, game_vars->window_size.w/2-game_vars->game_font->texture_size.x/2, 8);
    Game_SetFontText(game_vars->game_font, "ESC: Szimuláció menü, SPACE: Szimuláció szüneteltetése");
    Game_RenderFont(game_vars->game_font, game_vars->window_size.w/2-game_vars->game_font->texture_size.x/2, game_vars->window_size.h-game_vars->game_font->texture_size.y-8);
    Game_SetFontText(game_vars->game_font, "Bal egér: Cella felélesztése, Jobb egér: Cella megölése");
    Game_RenderFont(game_vars->game_font, game_vars->window_size.w/2-game_vars->game_font->texture_size.x/2, game_vars->window_size.h-game_vars->game_font->texture_size.y-32);
    SDL_RenderPresent(game_vars->renderer);

}
