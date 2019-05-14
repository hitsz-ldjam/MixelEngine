#pragma once

#ifndef MX_AUDIO_LISTENER_H_
#define MX_AUDIO_LISTENER_H_

#include "../Behaviour/MxBehaviour.h"
#include "../../Definitions/MxAudio.h"

namespace Mix {

    /**
     *  @note Each @code Scene @endcode can only have 1 @code AudioListener @endcode .
     */
    class AudioListener final : public Behaviour {
    MX_DECLARE_RTTI
    MX_DECLARE_CLASS_FACTORY

    public:
        AudioListener() : mListenerIdx(0),
                          mVelocityUpdateMode(Audio::VelocityUpdateMode::AUTO),
                          mCore(nullptr) {}

        ~AudioListener() = default;

        // void setVelocityUpdateMode()

    private:
        // static int sListenerNum; /**< Number of listeners. Obsolete. */
        int mListenerIdx; /**< Index for multiple listeners. Deprecated. */
        Audio::VelocityUpdateMode mVelocityUpdateMode;
        FMOD::System* mCore;

        void init() override;
        void update() override;
        void fixedUpdate() override;
    };
}

#endif
