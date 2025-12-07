#pragma once
#include <map>
#include <string>
#include <vector>
#include "../Dependencies/FMOD/inc/fmod.hpp"

#ifdef _WIN64
#pragma comment(lib, "../Dependencies/FMOD/lib/fmod_vc.lib")
#else
#pragma comment(lib, "../Dependencies/FMOD/lib/fmod_vc.lib")
#endif

namespace Core {
	struct SoundInfo {
		std::string name;
		std::string path;
		bool isBGM;
	};

	class SoundManager {
	public:
		static SoundManager& GetInstance() {
			static SoundManager instance;
			return instance;
		}
		void Initialize();
		void Play(const std::string name);
		void Stop(const std::string name);
		void StopAllSFX();
		void StopBGM();
		void Update();
		void CleanUp();

		void SetBGMVolume(float vol);
		void SetSFXVolume(float vol);

		float GetBGMVolume() { return bgmVolume; }
		float GetSFXVolume() { return sfxVolume; }

		void LoadSound(std::string name, std::string path, bool isBGM);
	private:
		SoundManager();
		~SoundManager() {}

		FMOD::System* system;		   // FMOD 사운드 시스템 (엔진)
		FMOD::Channel* channel;		   // 현재 재생 중인 채널 (임시 저장용)

		// --- 볼륨 조절을 위한 채널 그룹 ---
		FMOD::ChannelGroup* bgmGroup;  // 배경음악 그룹 믹서
		FMOD::ChannelGroup* sfxGroup;  // 효과음 그룹 믹서

		// --- 사운드 저장소 (이름, 사운드객체) ---
		std::map<std::string, FMOD::Sound*> bgmList; // 배경음악 목록
		std::map<std::string, FMOD::Sound*> sfxList; // 효과음 목록

		std::map<std::string, std::vector<FMOD::Channel*>> activeChannels;

		// --- 현재 볼륨 상태 (0.0 ~ 1.0) ---
		float bgmVolume;
		float sfxVolume;
	};
}