/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <stdio.h>
#include <fstream>
#include <iostream>


//==============================================================================
TSM1N3AudioProcessorEditor::TSM1N3AudioProcessorEditor (TSM1N3AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setOpaque (true);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to

    // Set Widget Graphics
    blackHexKnobLAF.setLookAndFeel(ImageCache::getFromMemory(BinaryData::knob2_png, BinaryData::knob2_pngSize));

    addAndMakeVisible(odFootSw);
    odFootSw.setImages(false, true, true,
        ImageCache::getFromMemory(BinaryData::footswitch_up_png, BinaryData::footswitch_up_pngSize), 1.0, Colours::transparentWhite,
        Image(), 1.0, Colours::transparentWhite,
        ImageCache::getFromMemory(BinaryData::footswitch_up_png, BinaryData::footswitch_up_pngSize), 1.0, Colours::transparentWhite,
        0.0);
    odFootSw.addListener(this);

    addAndMakeVisible(odLED);
    odLED.setImages(false, true, true,
        ImageCache::getFromMemory(BinaryData::led_red_on_png, BinaryData::led_red_on_pngSize), 1.0, Colours::transparentWhite,
        Image(), 1.0, Colours::transparentWhite,
        ImageCache::getFromMemory(BinaryData::led_red_on_png, BinaryData::led_red_on_pngSize), 1.0, Colours::transparentWhite,
        0.0);
    odLED.addListener(this);

    gainSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, GAIN_ID, ampGainKnob);
    addAndMakeVisible(ampGainKnob);
    ampGainKnob.setLookAndFeel(&blackHexKnobLAF);
    ampGainKnob.addListener(this);
    ampGainKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampGainKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    ampGainKnob.setDoubleClickReturnValue(true, 0.0);
	
    toneSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, TONE_ID, ampToneKnob);
    addAndMakeVisible(ampToneKnob);
    ampToneKnob.setLookAndFeel(&blackHexKnobLAF);
    ampToneKnob.addListener(this);
    ampToneKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampToneKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20 );
    ampToneKnob.setDoubleClickReturnValue(true, 0.5);

    masterSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, MASTER_ID, ampMasterKnob);
    addAndMakeVisible(ampMasterKnob);
    ampMasterKnob.setLookAndFeel(&blackHexKnobLAF);
    ampMasterKnob.addListener(this);
    ampMasterKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampMasterKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20 );
    ampMasterKnob.setDoubleClickReturnValue(true, 0.5);

    addAndMakeVisible(inputGainToggle);
    inputGainToggle.setLookAndFeel(&toggleSwitchLAF);
    inputGainToggle.setSliderStyle(Slider::SliderStyle::LinearVertical);
    inputGainToggle.setRange(0, 2, 1);
    inputGainToggle.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
    inputGainToggleAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, INPUT_GAIN_ID, inputGainToggle);

    addAndMakeVisible (infoButton);
    infoButton.addListener (this);
    addChildComponent (infoOverlay);

    // Size of plugin GUI
    setSize(255, 375);
    resetImages();
}

TSM1N3AudioProcessorEditor::~TSM1N3AudioProcessorEditor()
{
    ampGainKnob.setLookAndFeel(nullptr);
    ampToneKnob.setLookAndFeel(nullptr);
    ampMasterKnob.setLookAndFeel(nullptr);
    inputGainToggle.setLookAndFeel(nullptr);
}

//==============================================================================
void TSM1N3AudioProcessorEditor::paint (Graphics& g)
{
    g.drawImage (background, getLocalBounds().toFloat());

    g.setFont (FontOptions (10.0f, Font::bold));
    juce::String buildText = "RAMERED @ 26.6." + juce::String (__TIME__).substring (0, 5);

    // Draw main text (white)
    g.setColour (juce::Colours::white);
    g.drawSingleLineText (buildText, 1, 372);

    // Draw logo
    g.drawImage (logoImage, 40, 313, 32, 32, 0, 0, logoImage.getWidth(), logoImage.getHeight());

    // Draw handwritten labels for input gain toggle switch
    g.setFont (FontOptions ("Noteworthy", 9.0f, Font::plain));
    g.setColour (Colours::black.withAlpha (0.8f));
    g.drawText ("hi", 156, 95, 25, 10, Justification::right);
    g.drawText ("mid", 156, 105, 25, 10, Justification::right);
    g.drawText ("lo", 156, 115, 25, 10, Justification::right);
}

void TSM1N3AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    // Amp Widgets
    ampGainKnob.setBounds(15, 10, 94, 71);
    ampMasterKnob.setBounds(147, 10, 94, 71);
    ampToneKnob.setBounds(82, 68, 94, 71);

    inputGainToggle.setBounds(185, 92, 18, 37);

    odLED.setBounds(114, 236, 30, 30);
    odFootSw.setBounds(100, 274, 60, 60);

    infoButton.setBounds (229, 349, 16, 16);
    infoOverlay.setBounds (getLocalBounds());
}



void TSM1N3AudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &odFootSw) {
        if (audioProcessor.fw_state == 0)
            audioProcessor.fw_state = 1;
        else
            audioProcessor.fw_state = 0;

        resetImages();
    }
    else if (button == &infoButton)
    {
        infoOverlay.setVisible (true);
        infoOverlay.toFront (true);
    }
}


void TSM1N3AudioProcessorEditor::sliderValueChanged(Slider* /*slider*/)
{
    // Amp
    /*
    if (slider == &ampGainKnob)
        audioProcessor.setDrive(slider->getValue());
    else if (slider == &ampMasterKnob)
        audioProcessor.setMaster(slider->getValue());
    else if (slider == &ampToneKnob) 
        audioProcessor.setTone(slider->getValue());
        */
}


void TSM1N3AudioProcessorEditor::resetImages()
{
    if (audioProcessor.fw_state == 0) {
        odFootSw.setImages(false, true, true,
            ImageCache::getFromMemory(BinaryData::footswitch_up_png, BinaryData::footswitch_up_pngSize), 1.0, Colours::transparentWhite,
            Image(), 1.0, Colours::transparentWhite,
            ImageCache::getFromMemory(BinaryData::footswitch_up_png, BinaryData::footswitch_up_pngSize), 1.0, Colours::transparentWhite,
            0.0);
        odLED.setImages(false, true, true,
            ImageCache::getFromMemory(BinaryData::led_red_off_png, BinaryData::led_red_off_pngSize), 1.0, Colours::transparentWhite,
            Image(), 1.0, Colours::transparentWhite,
            ImageCache::getFromMemory(BinaryData::led_red_off_png, BinaryData::led_red_off_pngSize), 1.0, Colours::transparentWhite,
            0.0);
    }
    else {
        odFootSw.setImages(false, true, true,
            ImageCache::getFromMemory(BinaryData::footswitch_down_png, BinaryData::footswitch_down_pngSize), 1.0, Colours::transparentWhite,
            Image(), 1.0, Colours::transparentWhite,
            ImageCache::getFromMemory(BinaryData::footswitch_down_png, BinaryData::footswitch_down_pngSize), 1.0, Colours::transparentWhite,
            0.0);
       odLED.setImages(false, true, true,
            ImageCache::getFromMemory(BinaryData::led_red_on_png, BinaryData::led_red_on_pngSize), 1.0, Colours::transparentWhite,
            Image(), 1.0, Colours::transparentWhite,
            ImageCache::getFromMemory(BinaryData::led_red_on_png, BinaryData::led_red_on_pngSize), 1.0, Colours::transparentWhite,
            0.0);
    }
}