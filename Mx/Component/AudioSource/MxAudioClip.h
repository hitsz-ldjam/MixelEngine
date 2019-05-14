#pragma once

#ifndef MX_AUDIO_CLIP_H_
#define MX_AUDIO_CLIP_H_

//#define _CRT_SECURE_NO_WARNINGS

#include "../../Definitions/MxAudio.h"
#include "../../Exceptions/MxExceptions.hpp"

#include <filesystem>

namespace Mix {
    class AudioClip final {
        friend class AudioSource;

    public:
        /**
         *  @note Default constructor is only for Rtti. Do NOT use this manually.
         */
        AudioClip() : mCore(nullptr),
                      mSound(nullptr) {}

        AudioClip(const std::filesystem::path& _path,
                  const Audio::LoadType& _loadType = Audio::LoadType::DECOMPRESS_ON_LOAD)
            : mCore(Audio::Core()),
              mSound(nullptr),
              mPath(_path.string()),
              mLoadType(static_cast<FMOD_MODE>(_loadType)) {
            preloadAudioData = true;
            loadInBackground = false;
        }

        ~AudioClip();

        bool preloadAudioData; /**< Default is true. */
        bool loadInBackground; /**< Default is false. */

        // todo: implement
        // bool loadAudioData();

        Audio::LoadState loadState(unsigned* _percentBuffered = nullptr) const;

        float length() const;

    private:
        FMOD::System* mCore;
        FMOD::Sound* mSound;
        std::string mPath;
        FMOD_MODE mLoadType;

        // todo: un-inline this
        void loadAudioData(const bool _loop, const bool _3D) {
            if(mSound) return;

            auto loadMode = mLoadType;
            if(_loop)
                loadMode |= FMOD_LOOP_NORMAL;
            if(_3D)
                loadMode |= FMOD_3D;
            if(loadInBackground)
                loadMode |= FMOD_NONBLOCKING;

            auto result = mCore->createSound(mPath.c_str(), loadMode, nullptr, &mSound);
            if(result != FMOD_OK)
                throw FileLoadingError(mPath);
        }

        // Obsolete
        //void loadFileToMemory(const char* _name, char** _buffer, long* _length);
    };
}

#endif
