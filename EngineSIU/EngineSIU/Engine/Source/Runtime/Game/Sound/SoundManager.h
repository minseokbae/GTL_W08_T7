#pragma once

#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "Engine/Engine.h"
#include "Container/Map.h"

#include "ThirdParty/FMOD/include/fmod.hpp"

// 사운드 핸들러(엔진에 따라 FMOD::Sound*, OpenAL 핸들 등으로 변경)
using SoundHandle = FMOD::Sound*;

class USoundManager : public UObject
{
    DECLARE_CLASS(USoundManager, UObject)

private:
    FMOD::System* FmodSystem = nullptr;
    TMap<std::string, SoundHandle> SoundMap;
    TMap<std::string, FMOD::Channel*> ChannelMap;

public:
    USoundManager() = default;

    /** USoundManager를 가져옵니다. (싱글톤) */
    static USoundManager& Get()
    {
        if (USoundManager* Singleton = GEngine->SoundManager)
        {
            return *Singleton;
        }
        else
        {
            UE_LOG(ELogLevel::Error, "Cannot use MapManager if no SoundManagerClassName is defined!");
            assert(0);
            return *new USoundManager; // never calls this
        }
    }

    void InitSoundManager()
    {
        FMOD::System_Create(&FmodSystem);
        FmodSystem->init(512, FMOD_INIT_NORMAL, nullptr);

        LoadSound("piano", "Assets/Sounds/piano_loop.wav");
    }

    bool LoadSound(const std::string& Name, const std::string& FilePath)
    {
        if (!FmodSystem) return false;
        FMOD::Sound* sound = nullptr;
        FMOD_RESULT result = FmodSystem->createSound(FilePath.c_str(), FMOD_DEFAULT, nullptr, &sound);
        if (result == FMOD_OK && sound)
        {
            SoundMap.Add(Name, sound);
            return true;
        }
        return false;
    }

    void PlaySound(const std::string& Name, float Volume = 1.0f, bool Loop = false)
    {
        if (!FmodSystem) return;
        SoundHandle* soundPtr = SoundMap.Find(Name);
        if (soundPtr && *soundPtr)
        {
            FMOD::Channel* channel = nullptr;
            if (Loop)
                (*soundPtr)->setMode(FMOD_LOOP_NORMAL);
            else
                (*soundPtr)->setMode(FMOD_LOOP_OFF);

            FmodSystem->playSound(*soundPtr, nullptr, false, &channel);
            if (channel)
            {
                channel->setVolume(Volume);
                ChannelMap.Add(Name, channel);
            }
        }
    }

    void StopSound(const std::string& Name)
    {
        FMOD::Channel** channelPtr = ChannelMap.Find(Name);
        if (channelPtr && *channelPtr)
        {
            (*channelPtr)->stop();
        }
    }

    void SetVolume(const std::string& Name, float Volume)
    {
        FMOD::Channel** channelPtr = ChannelMap.Find(Name);
        if (channelPtr && *channelPtr)
        {
            (*channelPtr)->setVolume(Volume);
        }
    }

    void ReleaseAll()
    {
        for (auto& Elem : SoundMap)
        {
            if (Elem.Value)
                Elem.Value->release();
        }
        SoundMap.Empty();
        ChannelMap.Empty();
        if (FmodSystem)
        {
            FmodSystem->close();
            FmodSystem->release();
            FmodSystem = nullptr;
        }
    }

    bool IsSoundLoaded(const std::string& Name) const
    {
        return SoundMap.Contains(Name);
    }

    // Update 꼭 해줘야 한다던데 안 해도 되긴 함 이유 모름.
    void Update()
    {
        if (FmodSystem) FmodSystem->update();
    }
    // 필요에 따라 추가 기능 (채널별 제어, 3D 위치, 배경음/이펙트 분리 등)
};
