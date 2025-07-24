#include "pch.h"
#include "SDLVKSetup.h"

namespace Spunk
{
    SDLVKSetup* SDLVKSetup::Get()
    {
        if (!instance)
            instance = new SDLVKSetup();
        return instance;
    }
}
