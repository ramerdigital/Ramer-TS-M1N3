/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "myLookAndFeel.h"
#include <stdlib.h>

//==============================================================================
//==============================================================================
class InfoButton : public juce::Button {
public:
  InfoButton() : juce::Button("?") {}
  ~InfoButton() override = default;

  void paintButton(juce::Graphics &g, bool shouldDrawButtonAsHighlighted,
                   bool shouldDrawButtonAsDown) override {
    auto bounds = getLocalBounds().toFloat();

    // Draw background circle
    if (shouldDrawButtonAsDown)
      g.setColour(juce::Colours::white.withAlpha(0.4f));
    else if (shouldDrawButtonAsHighlighted)
      g.setColour(juce::Colours::white.withAlpha(0.25f));
    else
      g.setColour(juce::Colours::white.withAlpha(0.15f));

    g.fillEllipse(bounds);

    // Draw outline
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.drawEllipse(bounds, 1.0f);

    // Draw text "?"
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(bounds.getHeight() * 0.7f, juce::Font::bold));
    g.drawText("?", bounds, juce::Justification::centred);
  }
};

class CloseButton : public juce::Button {
public:
  CloseButton() : juce::Button("Close") {}
  ~CloseButton() override = default;

  void paintButton(juce::Graphics &g, bool shouldDrawButtonAsHighlighted,
                   bool shouldDrawButtonAsDown) override {
    auto bounds = getLocalBounds().toFloat();

    // Draw background (glassy transparent white)
    if (shouldDrawButtonAsDown)
      g.setColour(juce::Colours::white.withAlpha(0.25f));
    else if (shouldDrawButtonAsHighlighted)
      g.setColour(juce::Colours::white.withAlpha(0.15f));
    else
      g.setColour(juce::Colours::white.withAlpha(0.08f));

    g.fillRoundedRectangle(bounds, 3.0f);

    // Outline
    g.setColour(juce::Colours::white.withAlpha(0.4f));
    g.drawRoundedRectangle(bounds, 3.0f, 1.0f);

    // Text
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(9.0f, juce::Font::plain));
    g.drawText("Close", bounds, juce::Justification::centred);
  }
};

class InfoOverlay : public juce::Component, public juce::Button::Listener {
public:
  InfoOverlay() {
    // GitHub Link
    githubLink.setButtonText("https://github.com/ramerdigital/TS-M1N3");
    githubLink.setURL(juce::URL("https://github.com/ramerdigital/TS-M1N3"));
    githubLink.setFont(juce::FontOptions(9.0f, juce::Font::plain), false);
    githubLink.setJustificationType(juce::Justification::centredLeft);
    githubLink.setColour(juce::HyperlinkButton::textColourId,
                         juce::Colour(0xFF4DA6FF)); // Light blue
    githubLink.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    addAndMakeVisible(githubLink);

    // Privacy Link
    privacyLink.setButtonText("https://ramerdigital.com/Privacy");
    privacyLink.setURL(juce::URL("https://ramerdigital.com/Privacy"));
    privacyLink.setFont(juce::FontOptions(9.0f, juce::Font::plain), false);
    privacyLink.setJustificationType(juce::Justification::centredLeft);
    privacyLink.setColour(juce::HyperlinkButton::textColourId,
                          juce::Colour(0xFF4DA6FF)); // Light blue
    privacyLink.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    addAndMakeVisible(privacyLink);

    // Close Button
    closeButton.addListener(this);
    addAndMakeVisible(closeButton);
  }

  ~InfoOverlay() override = default;

  void paint(juce::Graphics &g) override {
    auto bounds = getLocalBounds();
    auto panelRect = bounds.reduced(14, 6);

    // Glass background
    g.setColour(juce::Colour(0xE61C1C1C)); // dark glass
    g.fillRoundedRectangle(panelRect.toFloat(), 8.0f);

    // Glass border highlight
    g.setColour(juce::Colours::white.withAlpha(0.15f));
    g.drawRoundedRectangle(panelRect.toFloat(), 8.0f, 1.2f);

    // Title
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(17.0f, juce::Font::bold));
    g.drawText("TS-M1N3 v" + juce::String(JucePlugin_VersionString), panelRect.getX(), panelRect.getY() + 10,
               panelRect.getWidth(), 20, juce::Justification::centred);

    // Subtitle
    g.setFont(juce::FontOptions(10.5f, juce::Font::plain));
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.drawText("by Ramer Digital", panelRect.getX(), panelRect.getY() + 30,
               panelRect.getWidth(), 12, juce::Justification::centred);

    // Divider
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.drawHorizontalLine(panelRect.getY() + 48, (float)(panelRect.getX() + 15),
                         (float)(panelRect.getRight() - 15));

    // Telemetry Policy Section
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(13.0f, juce::Font::bold));
    g.drawText("100% Free Plugin", panelRect.getX() + 15, panelRect.getY() + 56,
               panelRect.getWidth() - 30, 16, juce::Justification::left);

    g.setFont(juce::FontOptions(13.0f, juce::Font::plain));
    g.setColour(juce::Colours::white.withAlpha(0.7f));
    g.drawFittedText("This plugin runs offline. In line with Ramer Digital's "
                     "zero telemetry policy, it has zero tracking, zero "
                     "telemetry, and zero network connectivity.",
                     panelRect.getX() + 15, panelRect.getY() + 76,
                     panelRect.getWidth() - 30, 50, juce::Justification::left,
                     3);

    // Divider
    g.drawHorizontalLine(panelRect.getY() + 150, (float)(panelRect.getX() + 15),
                         (float)(panelRect.getRight() - 15));

    // Credits Section
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(13.0f, juce::Font::bold));
    g.drawText("Open Source Credits", panelRect.getX() + 15,
               panelRect.getY() + 158, panelRect.getWidth() - 30, 16,
               juce::Justification::left);

    g.setFont(juce::FontOptions(11.0f, juce::Font::plain));
    g.setColour(juce::Colours::white.withAlpha(0.7f));
    g.drawFittedText(
        "This project is built thanks to the generosity of the open source "
        "community. We credit the following libraries and their authors:",
        panelRect.getX() + 15, panelRect.getY() + 178,
        panelRect.getWidth() - 30, 48, juce::Justification::left, 3);

    // Submodules (using plain hyphens and spaced layout)
    g.setFont(juce::FontOptions(9.0f, juce::Font::plain));
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    int startY = panelRect.getY() + 240;
    g.drawSingleLineText("- TS-M1N3 Original (GPLv3) - GuitarML",
                         panelRect.getX() + 15, startY);
    g.drawSingleLineText("- JUCE Framework (GPLv3) - JUCE team",
                         panelRect.getX() + 15, startY + 11);
    g.drawSingleLineText("- RTNeural (BSD 3-Clause) - J. Chowdhury",
                         panelRect.getX() + 15, startY + 22);
    g.drawSingleLineText("- chowdsp_utils (GPLv3) - Chowdhury DSP",
                         panelRect.getX() + 15, startY + 33);
    g.drawSingleLineText("- r8brain-free-src (MIT) - A. Vaneev",
                         panelRect.getX() + 15, startY + 44);
    g.drawSingleLineText("- nlohmann/json (MIT) - N. Lohmann",
                         panelRect.getX() + 15, startY + 55);
  }

  void resized() override {
    auto bounds = getLocalBounds();
    auto panelRect = bounds.reduced(14, 6);

    privacyLink.setBounds(panelRect.getX() + 15, panelRect.getY() + 128,
                          panelRect.getWidth() - 30, 16);
    githubLink.setBounds(panelRect.getX() + 15, panelRect.getY() + 308,
                         panelRect.getWidth() - 30, 16);
    closeButton.setBounds(panelRect.getCentreX() - 25,
                          panelRect.getBottom() - 22, 50, 16);
  }

  void buttonClicked(juce::Button *button) override {
    if (button == &closeButton) {
      setVisible(false);
    }
  }

  void mouseDown(const juce::MouseEvent &) override {
    // Intercept clicks so they don't click knobs underneath
  }

  juce::HyperlinkButton privacyLink;
  juce::HyperlinkButton githubLink;
  CloseButton closeButton;
};

//==============================================================================
/**
 */
class TSM1N3AudioProcessorEditor : public AudioProcessorEditor,
                                   private Button::Listener,
                                   private Slider::Listener {
public:
  TSM1N3AudioProcessorEditor(TSM1N3AudioProcessor &);
  ~TSM1N3AudioProcessorEditor();

  //==============================================================================
  void paint(Graphics &) override;
  void resized() override;

  void resetImages();

  myLookAndFeel blackHexKnobLAF;

  struct ToggleSwitchLAF : public LookAndFeel_V4 {
    ToggleSwitchLAF() = default;
    void drawLinearSlider(Graphics &g, int x, int y, int width, int height,
                          float sliderPos, float, float,
                          const Slider::SliderStyle, Slider &) override {
      auto bounds = Rectangle<int>(x, y, width, height).toFloat();
      auto centerX = bounds.getCentreX();

      // Draw switch slot/track
      auto trackWidth = 3.0f;
      auto track =
          bounds.withSizeKeepingCentre(trackWidth, bounds.getHeight() * 0.7f);
      g.setColour(Colours::black.withAlpha(0.4f));
      g.fillRoundedRectangle(track, trackWidth * 0.5f);

      // Draw switch handle/lever
      auto handleHeight = 7.0f;
      auto handleWidth = 13.0f;
      auto handleRect = Rectangle<float>(centerX - handleWidth * 0.5f,
                                         sliderPos - handleHeight * 0.5f,
                                         handleWidth, handleHeight);

      // Draw metal lever body with vertical gradient
      ColourGradient cg(Colours::lightgrey, handleRect.getX(),
                        handleRect.getY(), Colours::darkgrey, handleRect.getX(),
                        handleRect.getBottom(), false);
      g.setGradientFill(cg);
      g.fillRoundedRectangle(handleRect, 2.0f);

      // Draw highlight line in the middle of the lever
      g.setColour(Colours::white.withAlpha(0.4f));
      g.drawHorizontalLine(static_cast<int>(sliderPos - 1.0f),
                           handleRect.getX() + 1.0f,
                           handleRect.getRight() - 1.0f);
      g.setColour(Colours::black.withAlpha(0.4f));
      g.drawHorizontalLine(static_cast<int>(sliderPos),
                           handleRect.getX() + 1.0f,
                           handleRect.getRight() - 1.0f);

      // Outline handle
      g.setColour(Colours::black.withAlpha(0.6f));
      g.drawRoundedRectangle(handleRect, 2.0f, 1.0f);
    }
  };
  ToggleSwitchLAF toggleSwitchLAF;

  ImageButton odFootSw;
  ImageButton odLED;

  InfoButton infoButton;
  InfoOverlay infoOverlay;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  TSM1N3AudioProcessor &audioProcessor;

  Image background =
      ImageCache::getFromMemory(BinaryData::ts_background_black_jpg,
                                BinaryData::ts_background_black_jpgSize);
  Image logoImage =
      ImageCache::getFromMemory(BinaryData::logo_png, BinaryData::logo_pngSize);

  // Amp Widgets
  Slider ampGainKnob;
  Slider ampMasterKnob;
  Slider ampToneKnob;
  Slider inputGainToggle;
  Slider pushToggle;

  virtual void buttonClicked(Button *button) override;

  // void updateToggleState(juce::Button* button, juce::String name);
  virtual void sliderValueChanged(Slider *slider) override;

  // AudioProcessorParameter* getParameter(const String& paramId);

  // float getParameterValue(const String& paramId);
  // void setParameterValue(const String& paramId, float value);

public:
  std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment>
      gainSliderAttach;
  std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment>
      toneSliderAttach;
  std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment>
      masterSliderAttach;
  std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment>
      inputGainToggleAttach;
  std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment>
      pushToggleAttach;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TSM1N3AudioProcessorEditor)
};
