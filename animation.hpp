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

    uint8_t drawEveryXFrame = 1;

    Animation(blit::Rect spriteLocation, blit::Point drawLocation, uint32_t frameLength,
        std::function<void()> callback = nullptr)
    {
        SetCoreProperties(spriteLocation, drawLocation, frameLength, callback);
    }

    Animation(blit::Rect spriteLocation, blit::Point drawLocation, uint32_t frameLength, uint8_t drawEveryXFrame,
        std::function<void()> callback = nullptr)
    {
        SetCoreProperties(spriteLocation, drawLocation, frameLength, callback);
        this->drawEveryXFrame = drawEveryXFrame;

    }

    Animation(blit::Rect spriteLocation, blit::Point drawLocation, uint32_t frameLength, int16_t rotation, Vec2 scale, int16_t rotationIncrement = 0, Vec2 scaleIncrement = Vec2(0, 0), //Actor* actor = NULL, 
        std::function<void()> callback = nullptr)
    {
        SetCoreProperties(spriteLocation, drawLocation, frameLength, callback);

        this->rotation = rotation;
        this->rotationIncrement = rotationIncrement;

        this->scale = scale;
        this->scaleIncrement = scaleIncrement;
        finalScale = scale += (scaleIncrement * frameLength);
    }

    void SetCoreProperties(const blit::Rect& spriteLocation, const blit::Point& drawLocation, const uint32_t& frameLength, std::function<void()>& callback)
    {
        this->spriteLocation = spriteLocation;
        this->drawLocation = drawLocation;
        this->frameLength = frameLength;
        this->callback = callback;       
    }

    void Animate();

    bool Draw();

    ~Animation();
};