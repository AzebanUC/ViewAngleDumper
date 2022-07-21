#pragma once

#include <Windows.h>

struct Vector3 {
    float X, Y, Z;

    void operator *= (float other) {
        X *= other;
        Y *= other;
        Z *= other;
    }
};

namespace Globals {
    inline UINT_PTR g_GameAssemblyBase;
}

namespace funcs {
    inline void(*Original_AddPunch)(UINT_PTR, Vector3, float);
}
