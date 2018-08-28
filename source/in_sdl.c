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

#include "cdaudio.h"
#include "client.h"
#include "common.h"
#include "console.h"
#include "cvar.h"
#include "input.h"
#include "keys.h"
#include "mathlib.h"
#include "quakedef.h"
#include "sdl_common.h"
#include "sound.h"
#include "sys.h"
#include "vid.h"

cvar_t in_snd_block = {"in_snd_block", "0"};

static qboolean mouse_available;
static int mouse_x, mouse_y;
static int joy_axis[4];
static SDL_Joystick *joystick[2];

static void JoyToKey(int button, int pressed) {
    static const int keymap[] = {
        /* KEY_A      */ K_ENTER,
        /* KEY_B      */ K_BACKSPACE,
        /* KEY_X      */ K_SPACE,
        /* KEY_Y      */ K_LCTRL,
        /* KEY_LSTICK */ K_UNKNOWN,
        /* KEY_RSTICK */ K_UNKNOWN,
        /* KEY_L      */ K_LSHIFT,
        /* KEY_R      */ K_RSHIFT,
        /* KEY_ZL     */ K_LALT,
        /* KEY_ZR     */ K_RALT,
        /* KEY_PLUS   */ K_ESCAPE,
        /* KEY_MINUS  */ K_TAB,
        /* KEY_DLEFT  */ K_LEFTARROW,
        /* KEY_DUP    */ K_UPARROW,
        /* KEY_DRIGHT */ K_RIGHTARROW,
        /* KEY_DDOWN  */ K_DOWNARROW,
    };

    if (button < 0 || button > 15) return;

    Key_Event(keymap[button], pressed);
}

void IN_ProcessEvents(void) {
    SDL_Event event;
    SDL_Keycode keycode;
    int keystate, button, keynum;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                keycode = event.key.keysym.sym;
                keystate = event.key.state;
                switch (keycode) {
                    case SDLK_UNKNOWN:
                        keynum = K_UNKNOWN;
                        break;
                    case SDLK_BACKSPACE:
                        keynum = K_BACKSPACE;
                        break;
                    case SDLK_TAB:
                        keynum = K_TAB;
                        break;
                    case SDLK_CLEAR:
                        keynum = K_CLEAR;
                        break;
                    case SDLK_RETURN:
                        keynum = K_ENTER;
                        break;
                    case SDLK_PAUSE:
                        keynum = K_PAUSE;
                        break;
                    case SDLK_ESCAPE:
                        keynum = K_ESCAPE;
                        break;
                    case SDLK_SPACE:
                        keynum = K_SPACE;
                        break;
                    case SDLK_EXCLAIM:
                        keynum = K_EXCLAIM;
                        break;
                    case SDLK_QUOTEDBL:
                        keynum = K_QUOTEDBL;
                        break;
                    case SDLK_HASH:
                        keynum = K_HASH;
                        break;
                    case SDLK_DOLLAR:
                        keynum = K_DOLLAR;
                        break;
                    case SDLK_AMPERSAND:
                        keynum = K_AMPERSAND;
                        break;
                    case SDLK_QUOTE:
                        keynum = K_QUOTE;
                        break;
                    case SDLK_LEFTPAREN:
                        keynum = K_LEFTPAREN;
                        break;
                    case SDLK_RIGHTPAREN:
                        keynum = K_RIGHTPAREN;
                        break;
                    case SDLK_ASTERISK:
                        keynum = K_ASTERISK;
                        break;
                    case SDLK_PLUS:
                        keynum = K_PLUS;
                        break;
                    case SDLK_COMMA:
                        keynum = K_COMMA;
                        break;
                    case SDLK_MINUS:
                        keynum = K_MINUS;
                        break;
                    case SDLK_PERIOD:
                        keynum = K_PERIOD;
                        break;
                    case SDLK_SLASH:
                        keynum = K_SLASH;
                        break;
                    case SDLK_0:
                        keynum = K_0;
                        break;
                    case SDLK_1:
                        keynum = K_1;
                        break;
                    case SDLK_2:
                        keynum = K_2;
                        break;
                    case SDLK_3:
                        keynum = K_3;
                        break;
                    case SDLK_4:
                        keynum = K_4;
                        break;
                    case SDLK_5:
                        keynum = K_5;
                        break;
                    case SDLK_6:
                        keynum = K_6;
                        break;
                    case SDLK_7:
                        keynum = K_7;
                        break;
                    case SDLK_8:
                        keynum = K_8;
                        break;
                    case SDLK_9:
                        keynum = K_9;
                        break;
                    case SDLK_COLON:
                        keynum = K_COLON;
                        break;
                    case SDLK_SEMICOLON:
                        keynum = K_SEMICOLON;
                        break;
                    case SDLK_LESS:
                        keynum = K_LESS;
                        break;
                    case SDLK_EQUALS:
                        keynum = K_EQUALS;
                        break;
                    case SDLK_GREATER:
                        keynum = K_GREATER;
                        break;
                    case SDLK_QUESTION:
                        keynum = K_QUESTION;
                        break;
                    case SDLK_AT:
                        keynum = K_AT;
                        break;
                    case SDLK_LEFTBRACKET:
                        keynum = K_LEFTBRACKET;
                        break;
                    case SDLK_BACKSLASH:
                        keynum = K_BACKSLASH;
                        break;
                    case SDLK_RIGHTBRACKET:
                        keynum = K_RIGHTBRACKET;
                        break;
                    case SDLK_CARET:
                        keynum = K_CARET;
                        break;
                    case SDLK_UNDERSCORE:
                        keynum = K_UNDERSCORE;
                        break;
                    case SDLK_BACKQUOTE:
                        keynum = K_BACKQUOTE;
                        break;
                    case SDLK_a:
                        keynum = K_a;
                        break;
                    case SDLK_b:
                        keynum = K_b;
                        break;
                    case SDLK_c:
                        keynum = K_c;
                        break;
                    case SDLK_d:
                        keynum = K_d;
                        break;
                    case SDLK_e:
                        keynum = K_e;
                        break;
                    case SDLK_f:
                        keynum = K_f;
                        break;
                    case SDLK_g:
                        keynum = K_g;
                        break;
                    case SDLK_h:
                        keynum = K_h;
                        break;
                    case SDLK_i:
                        keynum = K_i;
                        break;
                    case SDLK_j:
                        keynum = K_j;
                        break;
                    case SDLK_k:
                        keynum = K_k;
                        break;
                    case SDLK_l:
                        keynum = K_l;
                        break;
                    case SDLK_m:
                        keynum = K_m;
                        break;
                    case SDLK_n:
                        keynum = K_n;
                        break;
                    case SDLK_o:
                        keynum = K_o;
                        break;
                    case SDLK_p:
                        keynum = K_p;
                        break;
                    case SDLK_q:
                        keynum = K_q;
                        break;
                    case SDLK_r:
                        keynum = K_r;
                        break;
                    case SDLK_s:
                        keynum = K_s;
                        break;
                    case SDLK_t:
                        keynum = K_t;
                        break;
                    case SDLK_u:
                        keynum = K_u;
                        break;
                    case SDLK_v:
                        keynum = K_v;
                        break;
                    case SDLK_w:
                        keynum = K_w;
                        break;
                    case SDLK_x:
                        keynum = K_x;
                        break;
                    case SDLK_y:
                        keynum = K_y;
                        break;
                    case SDLK_z:
                        keynum = K_z;
                        break;
                    case SDLK_DELETE:
                        keynum = K_DEL;
                        break;
                    case SDLK_KP_0:
                        keynum = K_KP0;
                        break;
                    case SDLK_KP_1:
                        keynum = K_KP1;
                        break;
                    case SDLK_KP_2:
                        keynum = K_KP2;
                        break;
                    case SDLK_KP_3:
                        keynum = K_KP3;
                        break;
                    case SDLK_KP_4:
                        keynum = K_KP4;
                        break;
                    case SDLK_KP_5:
                        keynum = K_KP5;
                        break;
                    case SDLK_KP_6:
                        keynum = K_KP6;
                        break;
                    case SDLK_KP_7:
                        keynum = K_KP7;
                        break;
                    case SDLK_KP_8:
                        keynum = K_KP8;
                        break;
                    case SDLK_KP_9:
                        keynum = K_KP9;
                        break;
                    case SDLK_KP_PERIOD:
                        keynum = K_KP_PERIOD;
                        break;
                    case SDLK_KP_DIVIDE:
                        keynum = K_KP_DIVIDE;
                        break;
                    case SDLK_KP_MULTIPLY:
                        keynum = K_KP_MULTIPLY;
                        break;
                    case SDLK_KP_MINUS:
                        keynum = K_KP_MINUS;
                        break;
                    case SDLK_KP_PLUS:
                        keynum = K_KP_PLUS;
                        break;
                    case SDLK_KP_ENTER:
                        keynum = K_KP_ENTER;
                        break;
                    case SDLK_KP_EQUALS:
                        keynum = K_KP_EQUALS;
                        break;
                    case SDLK_UP:
                        keynum = K_UPARROW;
                        break;
                    case SDLK_DOWN:
                        keynum = K_DOWNARROW;
                        break;
                    case SDLK_RIGHT:
                        keynum = K_RIGHTARROW;
                        break;
                    case SDLK_LEFT:
                        keynum = K_LEFTARROW;
                        break;
                    case SDLK_INSERT:
                        keynum = K_INS;
                        break;
                    case SDLK_HOME:
                        keynum = K_HOME;
                        break;
                    case SDLK_END:
                        keynum = K_END;
                        break;
                    case SDLK_PAGEUP:
                        keynum = K_PGUP;
                        break;
                    case SDLK_PAGEDOWN:
                        keynum = K_PGDN;
                        break;
                    case SDLK_F1:
                        keynum = K_F1;
                        break;
                    case SDLK_F2:
                        keynum = K_F2;
                        break;
                    case SDLK_F3:
                        keynum = K_F3;
                        break;
                    case SDLK_F4:
                        keynum = K_F4;
                        break;
                    case SDLK_F5:
                        keynum = K_F5;
                        break;
                    case SDLK_F6:
                        keynum = K_F6;
                        break;
                    case SDLK_F7:
                        keynum = K_F7;
                        break;
                    case SDLK_F8:
                        keynum = K_F8;
                        break;
                    case SDLK_F9:
                        keynum = K_F9;
                        break;
                    case SDLK_F10:
                        keynum = K_F10;
                        break;
                    case SDLK_F11:
                        keynum = K_F11;
                        break;
                    case SDLK_F12:
                        keynum = K_F12;
                        break;
                    case SDLK_F13:
                        keynum = K_F13;
                        break;
                    case SDLK_F14:
                        keynum = K_F14;
                        break;
                    case SDLK_F15:
                        keynum = K_F15;
                        break;
                    case SDLK_NUMLOCKCLEAR:
                        keynum = K_NUMLOCK;
                        break;
                    case SDLK_CAPSLOCK:
                        keynum = K_CAPSLOCK;
                        break;
                    case SDLK_SCROLLLOCK:
                        keynum = K_SCROLLOCK;
                        break;
                    case SDLK_RSHIFT:
                        keynum = K_RSHIFT;
                        break;
                    case SDLK_LSHIFT:
                        keynum = K_LSHIFT;
                        break;
                    case SDLK_RCTRL:
                        keynum = K_RCTRL;
                        break;
                    case SDLK_LCTRL:
                        keynum = K_LCTRL;
                        break;
                    case SDLK_RALT:
                        keynum = K_RALT;
                        break;
                    case SDLK_LALT:
                        keynum = K_LALT;
                        break;
                    case SDLK_MODE:
                        keynum = K_MODE;
                        break;
                    case SDLK_HELP:
                        keynum = K_HELP;
                        break;
                    case SDLK_SYSREQ:
                        keynum = K_SYSREQ;
                        break;
                    case SDLK_MENU:
                        keynum = K_MENU;
                        break;
                    case SDLK_POWER:
                        keynum = K_POWER;
                        break;
                    case SDLK_UNDO:
                        keynum = K_UNDO;
                        break;
                    default:
                        keynum = K_UNKNOWN;
                        break;
                }
                Key_Event(keynum, keystate);

#ifdef DEBUG
                Sys_Printf(
                    "%s: SDL keycode = %s (%d), SDL scancode = %s (%d), "
                    "Quake key = %s (%d)\n",
                    __func__, SDL_GetKeyName(keycode), (int)keycode,
                    SDL_GetScancodeName(event.key.keysym.scancode), (int)event.key.keysym.scancode,
                    Key_KeynumToString(keynum), keynum);
#endif
                break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                button = event.button.button;
                if (button == 2)
                    button = 3;
                else if (button == 3)
                    button = 2;

                switch (button) {
                    case 1:
                    case 2:
                    case 3:
                        Key_Event(K_MOUSE1 + button - 1, event.type == SDL_MOUSEBUTTONDOWN);
                        break;
                }
                break;

            case SDL_MOUSEWHEEL:
                if (event.wheel.y < 0) {
                    while (event.wheel.y++) {
                        Key_Event(K_MWHEELDOWN, true);
                        Key_Event(K_MWHEELDOWN, false);
                    }
                } else if (event.wheel.y > 0) {
                    while (event.wheel.y--) {
                        Key_Event(K_MWHEELUP, true);
                        Key_Event(K_MWHEELUP, false);
                    }
                }
                break;

            case SDL_MOUSEMOTION:
                if (SDL_GetWindowGrab(sdl_window)) {
                    mouse_x += event.motion.xrel;
                    mouse_y += event.motion.yrel;
                }
                break;

            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
                JoyToKey(event.jbutton.button, event.type == SDL_JOYBUTTONDOWN);
                break;

            case SDL_JOYAXISMOTION:
                if (event.jaxis.axis >= 0 && event.jaxis.axis < 4)
                    joy_axis[event.jaxis.axis] = event.jaxis.value / 0x100;
                break;

            case SDL_QUIT:
                Sys_Quit();
                break;

            default:
                break;
        }
    }
}

static void IN_GrabMouse(int grab) {
    SDL_SetRelativeMouseMode(grab);
}

static void windowed_mouse_f(struct cvar_s *var) {}

static cvar_t m_filter = {"m_filter", "0"};
cvar_t _windowed_mouse = {"_windowed_mouse", "0", true, false, 0, windowed_mouse_f};

// FIXME - is this target independent?
static void IN_MouseMove(usercmd_t *cmd) {
    static float old_mouse_x, old_mouse_y;

    if (!mouse_available) return;

    if (m_filter.value) {
        mouse_x = (mouse_x + old_mouse_x) * 0.5;
        mouse_y = (mouse_y + old_mouse_y) * 0.5;
    }

    old_mouse_x = mouse_x;
    old_mouse_y = mouse_y;

    mouse_x *= sensitivity.value;
    mouse_y *= sensitivity.value;

    if ((in_strafe.state & 1) ||
        (lookstrafe.value && ((in_mlook.state & 1) ^ (int)m_freelook.value)))
        cmd->sidemove += m_side.value * mouse_x;
    else
        cl.viewangles[YAW] -= m_yaw.value * mouse_x;
    if ((in_mlook.state & 1) ^ (int)m_freelook.value)
        if (mouse_x || mouse_y) V_StopPitchDrift();

    if (((in_mlook.state & 1) ^ (int)m_freelook.value) && !(in_strafe.state & 1)) {
        cl.viewangles[PITCH] += m_pitch.value * mouse_y;
        if (cl.viewangles[PITCH] > 80) cl.viewangles[PITCH] = 80;
        if (cl.viewangles[PITCH] < -70) cl.viewangles[PITCH] = -70;
    } else {
        if ((in_strafe.state & 1) && noclip_anglehack)
            cmd->upmove -= m_forward.value * mouse_y;
        else
            cmd->forwardmove -= m_forward.value * mouse_y;
    }
    mouse_x = mouse_y = 0.0;
}

static void IN_JoyMove(usercmd_t *cmd) {
    int lx = joy_axis[0];
    int ly = joy_axis[1];
    int rx = joy_axis[2];
    int ry = joy_axis[3];

    // stolen from Rinnegatamante's vitaQuake

    int x_mov = abs(lx) < 30 ? 0 : (lx * cl_sidespeed.value) * 0.01;
    int y_mov = abs(ly) < 30 ? 0 : (ly * (ly > 0 ? cl_backspeed.value : cl_forwardspeed.value)) * 0.01;
    cmd->forwardmove -= y_mov;
    cmd->sidemove += x_mov;

    int x_cam = abs(rx) < 50 ? 0 : rx * sensitivity.value * 0.008;
    int y_cam = abs(ry) < 50 ? 0 : ry * sensitivity.value * 0.008;
    cl.viewangles[YAW] -= x_cam;

    V_StopPitchDrift();
    int invert = m_pitch.value < 0 ? -1 : 1;
    cl.viewangles[PITCH] += y_cam * invert;
}

void IN_Init(void) {
    int err;

    Q_SDL_InitOnce();

    err = SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    if (err < 0) Sys_Error("IN: Couldn't load SDL: %s", SDL_GetError());

#if 0
    SDL_EnableUNICODE(1); // Enable UNICODE translation for keyboard input
#endif

    joystick[0] = SDL_JoystickOpen(0);
    joystick[1] = SDL_JoystickOpen(1);

    mouse_x = mouse_y = 0.0;
    mouse_available = !COM_CheckParm("-nomouse");

    IN_GrabMouse(mouse_available);

    Cvar_RegisterVariable(&in_snd_block);
    Cvar_RegisterVariable(&m_filter);
    Cvar_RegisterVariable(&_windowed_mouse);
}

void IN_Shutdown(void) {
    if (joystick[0])
      SDL_JoystickClose(joystick[0]);
    if (joystick[1])
      SDL_JoystickClose(joystick[1]);

    mouse_available = 0;
}

/* Possibly don't need these? */
void IN_Accumulate(void) {}
void IN_UpdateClipCursor(void) {}
void IN_Move(usercmd_t *cmd) {
    IN_MouseMove(cmd);
    IN_JoyMove(cmd);
}

void IN_Commands(void) {
    IN_ProcessEvents();
}
