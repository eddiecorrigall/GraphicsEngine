#include "Input.hpp"
#include "Video.hpp"

#include <SDL2/SDL.h>

#include <iostream>

using namespace std;

Input::Input(const string& name) : System(name) {
    
    if (SDL_Init(SDL_INIT_JOYSTICK) != 0) {
        cerr << "ERROR: Failed to initialize SDL INPUT!" << endl;
        cerr << SDL_GetError() << endl;
        engine.Stop();
        return;
    }
    
    atexit(SDL_Quit);
}

Input::~Input() {
}

void Input::Update(const unsigned int& elapsed_milliseconds) {
    
    Video* gfx = SystemInstance<Video>();
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        
        switch (event.type) {
            
            case SDL_QUIT: {
                engine.Stop();
                return;
            }
            
            case SDL_WINDOWEVENT: {
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED: {
                        //video.Resize(event.window.data1, event.window.data2);
                        SystemInstance<Video>()->Resize(event.window.data1, event.window.data2);
                    } break;
                }
            } break;
            
            case SDL_KEYDOWN: {
                
                switch (event.key.keysym.sym) {
                    
                    case SDLK_ESCAPE: {
                        engine.Stop();
                        return;
                    } break;
                    
                    case SDLK_w: key_down[KEY_UP]       = true; break;
                    case SDLK_s: key_down[KEY_DOWN]     = true; break;
                    case SDLK_a: key_down[KEY_LEFT]     = true; break;
                    case SDLK_d: key_down[KEY_RIGHT]    = true; break;
                    
                    case SDLK_x: gfx->ToggleInterpolation(); break;
                    case SDLK_z: gfx->ToggleSubdivision(); break;
                    case SDLK_c: gfx->ToggleCelshading(); break;
                }
            } break;
            
            case SDL_KEYUP: {
                
                switch (event.key.keysym.sym) {
                    case SDLK_w: key_down[KEY_UP]       = false; break;
                    case SDLK_s: key_down[KEY_DOWN]     = false; break;
                    case SDLK_a: key_down[KEY_LEFT]     = false; break;
                    case SDLK_d: key_down[KEY_RIGHT]    = false; break;
                }
            } break;
        }
    }
    
    // ##### Camera ##### ///
    
    float elapsed_seconds = (float)elapsed_milliseconds / 1000.0f;
    float delta_frames = (float)VIDEO_FPS * elapsed_seconds;

    #define VIEW_MOVE_SPEED (delta_frames*5.0f)
    #define VIEW_TURN_SPEED (delta_frames*2.5f)
    
    const unsigned char* keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_GetScancodeFromKey(SDLK_w)]) {
        gfx->UpdateViewMove(0.0f, 0.0f, 0.0f + VIEW_MOVE_SPEED);
    }

    if (keys[SDL_GetScancodeFromKey(SDLK_s)]) {
        gfx->UpdateViewMove(0.0f, 0.0f, 0.0f - VIEW_MOVE_SPEED);
    }

    if (keys[SDL_GetScancodeFromKey(SDLK_a)]) {
        gfx->UpdateViewMove(0.0f - VIEW_MOVE_SPEED, 0.0f, 0.0f);
    }

    if (keys[SDL_GetScancodeFromKey(SDLK_d)]) {
        gfx->UpdateViewMove(0.0f + VIEW_MOVE_SPEED, 0.0f, 0.0f);
    }
    
    if (keys[SDL_GetScancodeFromKey(SDLK_LEFT)]) {
        gfx->UpdateViewPitch(0.0f + VIEW_TURN_SPEED);
    }
    
    if (keys[SDL_GetScancodeFromKey(SDLK_RIGHT)]) {
        gfx->UpdateViewPitch(0.0f - VIEW_TURN_SPEED);
    }
}
