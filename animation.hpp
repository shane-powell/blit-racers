#pragma once
#include <cstdint>
#include "32blit.hpp"
#include "types/rect.hpp"
//#include "actor.hpp"
using namespace blit;
class Animation {
public:
    blit::Rect spriteLocation;
    blit::Point drawLocation;

    uint32_t frameCount = 0;
    uint32_t frameLength = 1;
    float rotation = 0;
    //Actor* actor = NULL;
    bool incrementSprite = false;

    std::function<void()> callback;

    bool finished = false;

    Animation(blit::Rect spriteLocation, blit::Point drawLocation, uint32_t frameLength, int8_t rotation = 0, //Actor* actor = NULL, 
        std::function<void()> callback = NULL)
    {
        this->spriteLocation = spriteLocation;
        this->drawLocation = drawLocation;
        this->frameLength = frameLength;
        this->callback = callback;
        //this->actor = actor;
        this->rotation = rotation;
    }

    void Animate();

    ~Animation();
};