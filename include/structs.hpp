#pragma once

#ifdef GEODE_IS_WINDOWS
    #ifdef THREEDEE_API_EXPORTING
        #define THREEDEE_API_DLL __declspec(dllexport)
    #else
        #define THREEDEE_API_DLL __declspec(dllimport)
    #endif
#else
    #define THREEDEE_API_DLL __attribute__((visibility("default")))
#endif

namespace cocos2d {
    struct THREEDEE_API_DLL CCPoint3D { float x, y, z; };
}
