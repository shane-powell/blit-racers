#pragma once
#include <cstdint>
#include <32blit.hpp>
#include "types/rect.hpp"
using namespace blit;

class AnimationFrame {
public:

    Rect spriteLocation;
    Point drawLocation;

    uint32_t frameCount = 0;
    uint32_t frameLength = 1;

    std::function<void()> callback;

    bool finished = false;

    AnimationFrame(Rect spriteLocation, Point drawLocation, uint32_t frameLength, std::function<void()> callback = NULL)
    {
        this->spriteLocation = spriteLocation;
        this->drawLocation = drawLocation;
        this->frameLength = frameLength;
        this->callback = callback;
    }

    void Animate()
    {
        if (frameCount >= frameLength)
        {
            if (callback != NULL)
            {
                callback();
            }

            finished = true;
        }
        else
        {
            frameCount++;
        }
    }
};