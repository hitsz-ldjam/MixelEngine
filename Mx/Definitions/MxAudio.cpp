#include "MxAudio.h"

#include "../../MixEngine.h"

namespace Mix {
    FMOD::System* Audio::Core() {
        return MixEngine::Instance().mFmodCore;
    }
}
