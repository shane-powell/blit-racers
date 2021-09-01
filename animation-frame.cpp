#include "animation-frame.hpp"

void AnimationFrame::Animate()
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
