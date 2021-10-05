#include "animation.hpp"
#include "util.hpp"

void Animation::Animate()
{
    if (frameCount >= frameLength)
    {
        if (callback != nullptr)
        {
            callback();
        }

        finished = true;
    }
    else
    {
        /*if (this->actor != nullptr)
        {
            this->actor->degrees += this->rotation;
        }*/

        this->rotation = util::Rotate(this->rotation, rotationIncrement);
        this->scale += scaleIncrement;

        if (this->incrementSprite)
        {
            this->spriteLocation.x += 1;
        }

        frameCount++;
    }
}

Animation::~Animation()
{
    
}
