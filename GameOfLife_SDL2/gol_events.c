/**********************************
 * Project:         Game of Life
 * Filename:        gol_events.c
 * Description:     Az esemenykezelessel kapcsolatos fuggvenyek es definiciok
 *
 * Copyright Telek Istvan 2015
 *
 **********************************/
#include <time.h>
#include "gol_main.h"
#include "gol_grid.h"
#include "gol_file.h"
#include "gol_font.h"
#include "gol_events.h"

static MouseState mouse_state = MOUSESTATE_BOTH_UP;

static const char SAVE_FILE_NAME[] = "save/grid.txt";

/** \brief Segedfuggveny az ablak atmeretezesehez
 *
 * \param game_vars GameVars* A jatek fo valtozoja
 * \param x int Az uj szelesseg
 * \param y int Az uj magassag
 * \return void
 *
 */
static void Event_SetWindowSize(GameVars *game_vars, int x, int y) {
    game_vars->window_size.w = x;
    game_vars->window_size.h = y;
}

GameEvent events_parse(SDL_Event *ev) {
    switch (ev->type) {
    case SDL_KEYDOWN:
        switch (ev->key.keysym.sym) {
        case SDLK_ESCAPE:
            return EV_KEY_ESC;
        case SDLK_SPACE:
            return EV_KEY_SPACE;
        case SDLK_UP:
            return EV_KEY_UP;
        case SDLK_DOWN:
            return EV_KEY_DOWN;
        case SDLK_LEFT:
            return EV_KEY_LEFT;
        case SDLK_RIGHT:
            return EV_KEY_RIGHT;
        case SDLK_q:
            return EV_KEY_Q;
        default:
            return EV_INVALID;
        }
    case SDL_MOUSEBUTTONDOWN:
        switch (ev->button.button) {
        case SDL_BUTTON_LEFT:
            if (mouse_state == MOUSESTATE_RIGHT_DOWN) {
                mouse_state = MOUSESTATE_BOTH_DOWN;
            } else {
                mouse_state = MOUSESTATE_LEFT_DOWN;
            }
            return EV_MOUSE_L_D;
        case SDL_BUTTON_RIGHT:
            if (mouse_state == MOUSESTATE_LEFT_DOWN) {
                mouse_state = MOUSESTATE_BOTH_DOWN;
            } else {
                mouse_state = MOUSESTATE_RIGHT_DOWN;
            }
            return EV_MOUSE_R_D;
        default:
            break;
        }
        break;
    case SDL_MOUSEWHEEL:
        if (ev->wheel.y > 0) {
            return EV_MWHEELUP;
        } else if (ev->wheel.y < 0) {
            return EV_MWHEELDOWN;
        }
        break;
    case SDL_MOUSEBUTTONUP:
        switch (ev->button.button) {
        case SDL_BUTTON_LEFT:
            mouse_state = MOUSESTATE_BOTH_UP;
            return EV_MOUSE_L_U;
        case SDL_BUTTON_RIGHT:
            mouse_state = MOUSESTATE_BOTH_UP;
            return EV_MOUSE_R_U;
        }
    case SDL_MOUSEMOTION:
        return EV_MOUSE_MOTION;
    case SDL_WINDOWEVENT:
        switch (ev->window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            return EV_RESIZE;
            break;
        default:
            break;
        }
        break;
    case SDL_QUIT:
        return EV_EXIT;
    default:
        return EV_INVALID;
    }
    return EV_INVALID;
}

GameState events_menu_main(GameVars *game_vars) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        GameEvent gameev = events_parse(&ev);
        switch (gameev) {
        case EV_EXIT:
        case EV_KEY_ESC:
            return STATE_EXIT;
        case EV_KEY_SPACE:
            game_vars->grid_size.x = GRID_SIZE_DEFAULT;
            game_vars->grid_size.y = GRID_SIZE_DEFAULT;
            return STATE_NEW_MENU;
        case EV_KEY_DOWN:
            return STATE_LOAD_MENU;
        case EV_RESIZE:
            Event_SetWindowSize(game_vars, ev.window.data1, ev.window.data2);
            break;
        default:
            break;
        }
    }
    return game_vars->state;
}

GameState events_menu_load(GameVars *game_vars) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        GameEvent gameev = events_parse(&ev);
        switch (gameev) {
        case EV_EXIT:
            return STATE_EXIT;
        case EV_KEY_ESC:
            return STATE_MAIN_MENU;
        case EV_KEY_SPACE:
            if (game_vars->grid == NULL) {
                game_vars->grid = grid_new(-1, -1);
                grid_init(game_vars->grid);
                file_load_grid(SAVE_FILE_NAME, game_vars);
            }
            return STATE_SIM_PAUSED;
        case EV_RESIZE:
            Event_SetWindowSize(game_vars, ev.window.data1, ev.window.data2);
            break;
        default:
            break;
        }
    }
    return game_vars->state;
}

GameState events_menu_new(GameVars *game_vars) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        GameEvent gameev = events_parse(&ev);
        switch (gameev) {
        case EV_EXIT:
            return STATE_EXIT;
        case EV_KEY_ESC:
            return STATE_MAIN_MENU;
        case EV_KEY_SPACE:
            if (game_vars->grid == NULL) {
                game_vars->grid = grid_new(game_vars->grid_size.x, game_vars->grid_size.y);
                grid_init(game_vars->grid);
            }
            return STATE_SIM_PAUSED;
        case EV_KEY_UP:
            game_vars->grid_size.y++;
            break;
        case EV_KEY_DOWN:
            if (game_vars->grid_size.y > 1) {
                game_vars->grid_size.y--;
            }
            break;
        case EV_KEY_RIGHT:
            game_vars->grid_size.x++;
            break;
        case EV_KEY_LEFT:
            if (game_vars->grid_size.x > 1) {
                game_vars->grid_size.x--;
            }
            break;
        case EV_RESIZE:
            Event_SetWindowSize(game_vars, ev.window.data1, ev.window.data2);
            break;
        default:
            break;
        }
    }
    return game_vars->state;
}

GameState events_menu_sim_menu(GameVars *game_vars) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        GameEvent gameev = events_parse(&ev);
        switch (gameev) {
        case EV_EXIT:
            return STATE_EXIT;
        case EV_KEY_Q:
            if (game_vars->grid != NULL) {
                grid_free(game_vars->grid);
                game_vars->grid = NULL;
            }
            return STATE_MAIN_MENU;
        case EV_KEY_ESC:
            return STATE_SIM_PAUSED;
        case EV_KEY_DOWN:
            return STATE_SIM_SAVE;
        case EV_KEY_UP:
            return STATE_SIM_SETTINGS;
        case EV_RESIZE:
            Event_SetWindowSize(game_vars, ev.window.data1, ev.window.data2);
            break;
        default:
            break;
        }
    }
    return game_vars->state;
}

GameState events_menu_settings(GameVars *game_vars) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        GameEvent gameev = events_parse(&ev);
        switch (gameev) {
        case EV_EXIT:
            return STATE_EXIT;
        case EV_KEY_ESC:
            return STATE_SIM_MENU;
        case EV_RESIZE:
            Event_SetWindowSize(game_vars, ev.window.data1, ev.window.data2);
            break;
        default:
            break;
        }
    }
    return game_vars->state;
}

GameState events_menu_save(GameVars *game_vars) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        GameEvent gameev = events_parse(&ev);
        switch (gameev) {
        case EV_EXIT:
            return STATE_EXIT;
        case EV_KEY_ESC:
            return STATE_SIM_MENU;
        case EV_KEY_SPACE:
            file_save_grid(SAVE_FILE_NAME, game_vars);
            game_vars->settings.save_modified = time(0);
            return STATE_SIM_SAVE_CONF;
        case EV_RESIZE:
            Event_SetWindowSize(game_vars, ev.window.data1, ev.window.data2);
            break;
        default:
            break;
        }
    }
    return game_vars->state;
}

GameState events_menu_save_conf(GameVars *game_vars) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        GameEvent gameev = events_parse(&ev);
        switch (gameev) {
        case EV_EXIT:
            return STATE_EXIT;
        case EV_KEY_ESC:
            return STATE_SIM_MENU;
        case EV_RESIZE:
            Event_SetWindowSize(game_vars, ev.window.data1, ev.window.data2);
            break;
        default:
            break;
        }
    }
    return game_vars->state;
}

GameState events_sim_paused(GameVars *game_vars) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        GameEvent gameev = events_parse(&ev);
        switch (gameev) {
        case EV_EXIT:
            return STATE_EXIT;
        case EV_KEY_ESC:
            return STATE_SIM_MENU;
        case EV_KEY_SPACE:
            return STATE_SIM_RUNNING;
        case EV_KEY_RIGHT:
            grid_logic(game_vars->grid);
            break;
        case EV_MOUSE_L_D:
            if (mouse_state == MOUSESTATE_LEFT_DOWN) {
                grid_set_alive(game_vars->grid, ev.motion.x/game_vars->settings.cell_size, ev.motion.y/game_vars->settings.cell_size);
            }
            break;
        case EV_MOUSE_R_D:
            if (mouse_state == MOUSESTATE_RIGHT_DOWN) {
                grid_set_dead(game_vars->grid, ev.motion.x/game_vars->settings.cell_size, ev.motion.y/game_vars->settings.cell_size);
            }
            break;
        case EV_MOUSE_MOTION:
            switch (mouse_state) {
            case MOUSESTATE_LEFT_DOWN:
                grid_set_alive(game_vars->grid, ev.motion.x/game_vars->settings.cell_size, ev.motion.y/game_vars->settings.cell_size);
                break;
            case MOUSESTATE_RIGHT_DOWN:
                grid_set_dead(game_vars->grid, ev.motion.x/game_vars->settings.cell_size, ev.motion.y/game_vars->settings.cell_size);
                break;
            default:
                break;
            }
            break;
        case EV_MWHEELUP:
            if (game_vars->settings.cell_size < 1024) {
                game_vars->settings.cell_size*=2;
            }
            break;
        case EV_MWHEELDOWN:
            if (game_vars->settings.cell_size > 1) {
                game_vars->settings.cell_size/=2;
            }
            break;
        case EV_RESIZE:
            Event_SetWindowSize(game_vars, ev.window.data1, ev.window.data2);
            break;
        default:
            break;
        }
    }
    return game_vars->state;
}

GameState events_sim_running(GameVars *game_vars) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        GameEvent gameev = events_parse(&ev);
        switch (gameev) {
        case EV_EXIT:
            return STATE_EXIT;
        case EV_KEY_ESC:
            return STATE_SIM_MENU;
        case EV_KEY_SPACE:
            return STATE_SIM_PAUSED;
        case EV_MOUSE_L_D:
            grid_set_alive(game_vars->grid, ev.motion.x/game_vars->settings.cell_size, ev.motion.y/game_vars->settings.cell_size);
            break;
        case EV_MOUSE_R_D:
            grid_set_dead(game_vars->grid, ev.motion.x/game_vars->settings.cell_size, ev.motion.y/game_vars->settings.cell_size);
            break;
        case EV_MOUSE_MOTION:
            switch (mouse_state) {
            case MOUSESTATE_LEFT_DOWN:
                grid_set_alive(game_vars->grid, ev.motion.x/game_vars->settings.cell_size, ev.motion.y/game_vars->settings.cell_size);
                break;
            case MOUSESTATE_RIGHT_DOWN:
                grid_set_dead(game_vars->grid, ev.motion.x/game_vars->settings.cell_size, ev.motion.y/game_vars->settings.cell_size);
                break;
            default:
                break;
            }
            break;
        case EV_MWHEELUP:
            if (game_vars->settings.cell_size < 1024) {
                game_vars->settings.cell_size*=2;
            }
            break;
        case EV_MWHEELDOWN:
            if (game_vars->settings.cell_size > 1) {
                game_vars->settings.cell_size/=2;
            }
            break;
        case EV_RESIZE:
            Event_SetWindowSize(game_vars, ev.window.data1, ev.window.data2);
            break;
        default:
            break;
        }
    }
    return game_vars->state;
}
