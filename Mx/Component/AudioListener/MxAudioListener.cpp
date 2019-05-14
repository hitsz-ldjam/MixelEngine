#include "MxAudioListener.h"

#include "../../GameObject/MxGameObject.h"

namespace Mix {
    MX_IMPLEMENT_RTTI_NO_CREATE_FUNC(AudioListener, Behaviour)
    MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(AudioListener)

    void AudioListener::init() {
        if(mCore) return;

        mCore = Audio::Core();
        if(!mCore)
            throw AudioManagerNotInitializedError();

        // mListenerIdx = sListenerNum;
    }

    void AudioListener::update() {
        if(!mGameObject)
            throw IndependentComponentError();

        auto trans = mGameObject->transform();

        FMOD_VECTOR pos = Audio::glmVec3ToFmodVec(trans.position()),
                    vel = {0, 0, 0},
                    forward = Audio::glmVec3ToFmodVec(trans.forward()),
                    up = Audio::glmVec3ToFmodVec(trans.up());

        // todo: set up velocity
        //Rigidbody* rb = mGameObject->getComponent<Rigidbody>();
        //if(rb)
        //    vel = Utils::glmVec3ToFmodVec(rb->velocity());

        mCore->set3DListenerAttributes(mListenerIdx, &pos, &vel, &forward, &up);
    }

    void AudioListener::fixedUpdate() {}
}
