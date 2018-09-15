/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include <SDL2/SDL.h>

#include "qtypes.h"
#include "sdl_common.h"
#include "sys.h"

SDL_Window *sdl_window = NULL;

void Q_SDL_InitOnce(void) {
    static qboolean init_done = false;

    if (init_done) return;

    if (SDL_Init(SDL_INIT_EVENTS) < 0) Sys_Error("SDL_Init(0) failed: %s", SDL_GetError());
#ifdef GLQUAKE
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) printf("SDL_VIDEO borked: %s\n", SDL_GetError());
    printf("sdl_window\n");
    int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN;
    sdl_window = SDL_CreateWindow("", 0, 0, 1280, 720, flags);
    if (!sdl_window) printf("sdl_window == NULL!\n%s\n", SDL_GetError());
    SDL_Renderer *renderer = SDL_CreateRenderer(sdl_window, -1, 0);
    if (!renderer) printf("sdl_renderer == NULL!\n%s\n", SDL_GetError());
#endif

    init_done = true;
}
