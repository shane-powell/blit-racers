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
    int16_t rotation = 0;
    int16_t rotationIncrement = 0;
    //Actor* actor = nullptr;
    bool incrementSprite = false;

    blit::Vec2 scale = Vec2 (1,1);
    blit::Vec2 finalScale = Vec2(1, 1);
    Vec2 scaleIncrement = Vec2(0, 0);

    std::function<void()> callback;

    bool finished = false;

    Animation(blit::Rect spriteLocation, blit::Point drawLocation, uint32_t frameLength,  //Actor* actor = nullptr, 
        std::function<void()> callback = nullptr)
    {
        this->spriteLocation = spriteLocation;
        this->drawLocation = drawLocation;
        this->frameLength = frameLength;
        this->callback = callback;
        //this->actor = actor;
        this->rotation = rotation;
        this->rotationIncrement = rotationIncrement;

    }

    Animation(blit::Rect spriteLocation, blit::Point drawLocation, uint32_t frameLength, int16_t rotation, Vec2 scale, int16_t rotationIncrement = 0, Vec2 scaleIncrement = Vec2(0, 0), //Actor* actor = NULL, 
        std::function<void()> callback = nullptr)
    {
        this->spriteLocation = spriteLocation;
        this->drawLocation = drawLocation;
        this->frameLength = frameLength;
        this->callback = callback;
        //this->actor = actor;
        this->rotation = rotation;
        this->rotationIncrement = rotationIncrement;
        this->scale = scale;
        this->scaleIncrement = scaleIncrement;

       /* for (int i = 0; i < frameLength; i++)
        {
            finalScale += scaleIncrement;
        }*/

        finalScale = scale += (scaleIncrement * frameLength);
    }

    void Animate();

    ~Animation();
};