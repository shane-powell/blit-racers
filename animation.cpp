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


       /* if (frameCount % this->drawEveryXFrame == 0)
        {*/
            this->rotation = util::Rotate(this->rotation, rotationIncrement);
            this->scale += scaleIncrement;

            if (this->incrementSprite)
            {
                this->spriteLocation.x += 1;
            }
       // }

        frameCount++;
    }
}

bool Animation::Draw()
{
    return this->frameCount % this->drawEveryXFrame == 0;
}

Animation::~Animation()
{
    
}
