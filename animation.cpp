#include "animation.hpp"

void Animation::Animate()
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
        if (this->actor != NULL)
        {
            this->actor->degrees += this->rotation;
        }

        if (this->incrementSprite)
        {
            this->spriteLocation.x += 1;
        }

        frameCount++;
    }
}
