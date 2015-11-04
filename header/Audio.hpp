#ifndef __AUDIO_HPP__
#define __AUDIO_HPP__

#include "Process.hpp"

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define MAX_SOUND (100)
#define MAX_MUSIC (100)

class Audio : public System {
	
	private:
		
		int sound_count;
		int music_count;
		
		Mix_Chunk* sound_chunks[MAX_SOUND];
		int sound_channels[MAX_SOUND];
		
		Mix_Music* music[MAX_MUSIC];
		
	protected:
		
		void Update(const unsigned int& elapsed_milliseconds);
		
		void ResetSound(const int& sound_id);
		void ResetMusic(const int& music_id);
		
	public:
		
		Audio(const std::string& name);
		~Audio();
		
		// Return -1 on failure
		// sound file -> sound id
		int LoadSound(const std::string& sound_file);
		
		bool IsSound(const int& sound_id);
		void PlaySound(const int& sound_id);
		void StopSound(const int& sound_id);
		void PauseSound(const int& sound_id);
		void ResumeSound(const int& sound_id);
		
		// Return -1 on failure
		// music file -> sound id
		int LoadMusic(const std::string& music_file);
		
		bool IsMusic(const int& music_id);
		void PlayMusic(const int& music_id, const bool& loop);
		void StopMusic(const int& music_id);
		void PauseMusic(const int& music_id);
		void ResumeMusic(const int& music_id);
};

#endif
