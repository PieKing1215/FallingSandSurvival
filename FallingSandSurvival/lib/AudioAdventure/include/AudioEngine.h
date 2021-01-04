#ifndef _AUDIO_ENGINE_H_
#define _AUDIO_ENGINE_H_

#include "fmod_studio.hpp"
#include "fmod.hpp"
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>

using namespace std;

struct Vector3 {
	float x;
	float y;
	float z;
};

struct Implementation {
	Implementation();
	~Implementation();

	void Update();
	
	FMOD::Studio::System* mpStudioSystem;
	FMOD::System* mpSystem;

	int mnNextChannelId;

	typedef map<string, FMOD::Sound*> SoundMap;
	typedef map<int, FMOD::Channel*> ChannelMap;
	typedef map<string, FMOD::Studio::EventInstance*> EventMap;
	typedef map<string, FMOD::Studio::Bank*> BankMap;
	BankMap mBanks;
	EventMap mEvents;
	SoundMap mSounds;
	ChannelMap mChannels;
};

class CAudioEngine {
public:
	static void Init();
	static void Update();
	static void Shutdown();
	static int ErrorCheck(FMOD_RESULT result);

	void LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
    FMOD::Studio::Bank* GetBank(const std::string& strBankName);
	void LoadEvent(const std::string& strEventName);
	void LoadSound(const string &strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);
	void UnLoadSound(const string &strSoundName);
	void Set3dListenerAndOrientation(const Vector3& vPosition, const Vector3& vLook, const Vector3& vUp);
	int PlaySounds(const string &strSoundName, const Vector3& vPos = Vector3{ 0, 0, 0 }, float fVolumedB = 0.0f);
	void PlayEvent(const string &strEventName);
	FMOD::Studio::EventInstance* GetEvent(const string &strEventName);
	void StopChannel(int nChannelId);
	void StopEvent(const string &strEventName, bool bImmediate = false);
	void GetEventParameter(const string &strEventName, const string &strEventParameter, float* parameter);
	void SetEventParameter(const string &strEventName, const string &strParameterName, float fValue);
	void SetGlobalParameter(const string &strParameterName, float fValue);
	void GetGlobalParameter(const string& strEventParameter, float* parameter);
	void StopAllChannels();
	void SetChannel3dPosition(int nChannelId, const Vector3& vPosition);
	void SetChannelVolume(int nChannelId, float fVolumedB);
	bool IsPlaying(int nChannelId) const;
	bool IsEventPlaying(const string &strEventName) const;
	float dbToVolume(float dB);
	float VolumeTodB(float volume);
	FMOD_VECTOR VectorToFmod(const Vector3& vPosition);
};

#endif
