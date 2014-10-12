#include "Audio.hpp"

#include <string>
#include <iostream>

#include <cassert>

Audio::Audio(const std::string& name) : System(name) {
    
    sound_count = 0;
    music_count = 0;
    
    if (SDL_Init(SDL_INIT_AUDIO) != 0) {
        std::cerr << "ERROR: Failed to initialize SDL AUDIO!" << std::endl;
        std::cerr << SDL_GetError() << std::endl;
        return;
    }
    
    atexit(SDL_Quit);
    
    #define AUDIO_RATE      ((int)22050)
    #define AUDIO_FORMAT    ((unsigned short)AUDIO_S16)
    #define AUDIO_CHANNELS  ((int)2)
    #define AUDIO_BUFFERS   ((int)4096)
    
    if(Mix_OpenAudio(AUDIO_RATE, AUDIO_FORMAT, AUDIO_CHANNELS, AUDIO_BUFFERS)) {
        std::cerr << "ERROR: Failed to open SDL AUDIO!" << std::endl;
        std::cerr << SDL_GetError() << std::endl;
        return;
    }
}

Audio::~Audio() {
    
    for (int sound_id = 0; sound_id < MAX_SOUND; sound_id++) {
        if (IsSound(sound_id)) {
            ResetSound(sound_id);
        }
    }
    
    for (int music_id = 0; music_id < MAX_MUSIC; music_id++) {
        if (IsMusic(music_id)) {
            ResetMusic(music_id);
        }
    }
}

void Audio::Update(const unsigned int& elapsed_milliseconds) {
    /*
    if (data != NULL) {
        std::vector<Entity*>* entities = (std::vector<Entity*>*)data;
        // TODO: ...
    }
    */
}

bool Audio::IsSound(const int& sound_id) {
    
    if (sound_id < 0) return false;
    if (MAX_SOUND <= sound_id) return false;
    if (sound_chunks[sound_id] == NULL) return false;
    
    return true;
}

void Audio::ResetSound(const int& sound_id) {
    
    assert(IsSound(sound_id));
    
    Mix_HaltChannel(sound_channels[sound_id]);
    sound_channels[sound_id] = -1;
}

int Audio::LoadSound(const std::string& sound_file) {
    
    const int sound_id = sound_count;
    
    if (sound_id >= MAX_SOUND) {
        std::cerr << "ERROR: Sound registry full!" << std::endl;
        return -1;
    }
    
    Mix_Chunk* buffer = Mix_LoadWAV(sound_file.c_str());
    
    if (buffer == NULL) {
        std::cerr << "ERROR: Failed to load " << sound_file << std::endl;
        std::cerr << SDL_GetError() << std::endl;
        return -1;
    }
    
    sound_chunks[sound_id] = buffer;
    sound_channels[sound_id] = -1;
    sound_count++;
    
    return sound_id;
}

void Audio::PlaySound(const int& sound_id) {
    
    assert(IsSound(sound_id));
    
    sound_channels[sound_id] = Mix_PlayChannel(-1, sound_chunks[sound_id], 0);
}

void Audio::StopSound(const int& sound_id) {
    
    assert(IsSound(sound_id));
    
    ResetSound(sound_id);
}

void Audio::PauseSound(const int& sound_id) {
    
    assert(IsSound(sound_id));
    
    Mix_Pause(sound_channels[sound_id]);
}

void Audio::ResumeSound(const int& sound_id) {
    
    assert(IsSound(sound_id));
    
    Mix_Resume(sound_channels[sound_id]);
}

bool Audio::IsMusic(const int& music_id) {
    
    if (music_id < 0) return false;
    if (MAX_MUSIC <= music_id) return false;
    if (music[music_id] == NULL) return false;
    
    return true;
}

void Audio::ResetMusic(const int& music_id) {
    
    assert(IsMusic(music_id));
    
    Mix_HaltMusic();
}

int Audio::LoadMusic(const std::string& music_file) {
    
    const int music_id = music_count;
    
    if (music_id >= MAX_MUSIC) {
        std::cerr << "ERROR: Music registry is full!" << std::endl;
        return -1;
    }
    
    Mix_Music* buffer = Mix_LoadMUS(music_file.c_str());
    
    if (buffer == NULL) {
        std::cerr << "ERROR: Failed to load " << music_file << std::endl;
        std::cerr << SDL_GetError() << std::endl;
        return -1;
    }
    
    music[music_id] = buffer;
    music_count++;
    
    return music_id;
}

void Audio::PlayMusic(const int& music_id, const bool& loop) {
    
    assert(IsMusic(music_id));
    
    Mix_PlayMusic(music[music_id], loop ? -1 : 1);
}

void Audio::StopMusic(const int& music_id) {
    
    assert(IsMusic(music_id));
    
    ResetMusic(music_id);
}

void Audio::PauseMusic(const int& music_id) {
    
    assert(IsMusic(music_id));
    
    Mix_PauseMusic();
}

void Audio::ResumeMusic(const int& music_id) {
    
    assert(IsMusic(music_id));
    
    Mix_ResumeMusic();
}
