#include "MxAudioSource.h"

namespace Mix {
    MX_IMPLEMENT_RTTI_NO_CREATE_FUNC(AudioSource, Behaviour)
    MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(AudioSource)

    //todo

    void AudioSource::play() {
        if(!mClip || !mClip->mSound)
            return;
        FMOD::System* core = nullptr;
        mClip->mSound->getSystemObject(&core);
        core->playSound(mClip->mSound, nullptr, false, &mChannel);
    }

    void AudioSource::init() {
        auto mCore = Audio::Core();
        if(mClip->preloadAudioData)
            // todo: alter whether load as 3d sound
            mClip->loadAudioData(loop, true);
        mCore->playSound(mClip->mSound, nullptr, !playOnAwake, &mChannel);
        //mCore->playSound(mClip->mSound, nullptr, !playOnAwake, &mChannel);
    }
}
