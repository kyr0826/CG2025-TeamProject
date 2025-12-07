#include "SoundManager.h"
#include "Common_Headers.h"

namespace Core {
	void SoundManager::Initialize() {
		FMOD::System_Create(&system);
		system->init(32, FMOD_INIT_NORMAL, nullptr);

		// 볼륨 제어를 위한 채널 그룹 생성
		system->createChannelGroup("BGM", &bgmGroup);
		system->createChannelGroup("SFX", &sfxGroup);
	}
	void SoundManager::Play(const std::string name) {
		// 1. BGM 목록에서 검색 -> bgmGroup으로 재생
		if (bgmList.find(name) != bgmList.end()) {
			system->playSound(bgmList[name], bgmGroup, false, &channel);
			activeChannels[name].clear();
			activeChannels[name].push_back(channel);
			return;
		}

		// 2. SFX 목록에서 검색 -> sfxGroup으로 재생
		if (sfxList.find(name) != sfxList.end()) {
			system->playSound(sfxList[name], sfxGroup, false, &channel);
			activeChannels[name].push_back(channel);
			return;
		}

		std::cout << "Sound Not Found: " << name << std::endl;
	}

	void SoundManager::Stop(const std::string name) {
		if (activeChannels.find(name) == activeChannels.end()) return;

		for (auto* ch : activeChannels[name]) {
			if (ch) ch->stop();
		}
		activeChannels[name].clear();
	}

	void SoundManager::StopAllSFX() {
		if (sfxGroup) sfxGroup->stop();
	}

	void SoundManager::StopBGM() {
		if (bgmGroup) bgmGroup->stop();
	}

	void SoundManager::Update() {
		if (!system) return;
		system->update();
	}

	void SoundManager::CleanUp() {
		// 사운드 리소스 해제
		for (auto& pair : bgmList) pair.second->release();
		for (auto& pair : sfxList) pair.second->release();
		bgmList.clear();
		sfxList.clear();

		// 그룹 해제
		if (bgmGroup) bgmGroup->release();
		if (sfxGroup) sfxGroup->release();

		// 시스템 종료
		if (system) {
			system->close();
			system->release();
		}
	}

	void SoundManager::SetBGMVolume(float vol) {
		// 0.0 ~ 1.0 범위 제한 (Clamp)
		if (vol < 0.0f) vol = 0.0f;
		if (vol > 1.0f) vol = 1.0f;

		bgmVolume = vol;
		bgmGroup->setVolume(bgmVolume);
	}
	void SoundManager::SetSFXVolume(float vol) {
		if (vol < 0.0f) vol = 0.0f;
		if (vol > 1.0f) vol = 1.0f;

		sfxVolume = vol;
		sfxGroup->setVolume(sfxVolume);
	}

	SoundManager::SoundManager() : system(nullptr), channel(nullptr), bgmGroup(nullptr), sfxGroup(nullptr),
		bgmVolume(1.0f), sfxVolume(1.0f) {
	}

	void SoundManager::LoadSound(std::string name, std::string path, bool isBGM) {
		FMOD::Sound* sound = nullptr;

		if (isBGM) {
			// BGM: 이미 있으면 패스, 스트리밍+루프 모드로 로드
			if (bgmList.find(name) != bgmList.end()) return;
			system->createSound(path.c_str(), FMOD_LOOP_NORMAL, nullptr, &sound);

			if (sound) {
				bgmList[name] = sound;
				std::cout << "BGM Loaded: " << name << std::endl;
			}
		}
		else {
			// SFX: 이미 있으면 패스, 메모리 로드 모드
			if (sfxList.find(name) != sfxList.end()) return;
			system->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &sound);

			if (sound) {
				sfxList[name] = sound;
				std::cout << "SFX Loaded: " << name << std::endl;
			}
		}
	}
}