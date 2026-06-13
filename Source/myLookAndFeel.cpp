/*
  ==============================================================================

    Created: 14 Dec 2017 10:16:04am
    Author:  Stefan Remberg

    Modified by keyth72

  ==============================================================================
*/

#include "myLookAndFeel.h"

//==============================================================================
myLookAndFeel::myLookAndFeel()
{
}

//==============================================================================
void myLookAndFeel::setLookAndFeel(Image inputImage)
{
    img = inputImage;
}


//==============================================================================
void myLookAndFeel::drawRotarySlider(Graphics& g,
    int x, int y, int width, int height, float /*sliderPos*/,
    float /*rotaryStartAngle*/, float /*rotaryEndAngle*/, Slider& slider)
{
    const double rotation = (slider.getValue()
        - slider.getMinimum())
        / (slider.getMaximum()
            - slider.getMinimum());

    const int frames = img.getHeight() / img.getWidth();
    const int frameId = (int)ceil(rotation * ((double)frames - 1.0));
    const auto fWidth = static_cast<float> (width);
    const auto fHeight = static_cast<float> (height);
    const auto fX = static_cast<float> (x);
    const auto fY = static_cast<float> (y);
    const float radius = jmin(fWidth / 2.0f, fHeight / 2.0f);
    const float centerX = fX + fWidth * 0.5f;
    const float centerY = fY + fHeight * 0.5f;
    const float rx = centerX - radius - 1.0f;
    const float ry = centerY - radius;

    g.drawImage(img,
        (int)rx,
        (int)ry,
        2 * (int)radius,
        2 * (int)radius,
        0,
        frameId*img.getWidth(),
        img.getWidth(),
        img.getWidth());
}
