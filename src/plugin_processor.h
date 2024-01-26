#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include "BinaryData.h"
#include "distortion/distortion.h"
#include "tremolo/tremolo.h"
#include "parameters/globals.h"

class AudioPluginAudioProcessor final
    : public juce::AudioProcessor,
      juce::AudioProcessorValueTreeState::Listener {
   public:
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    void prepareToPlay(double sample_rate, int samples_per_block) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& new_name) override;

    void getStateInformation(juce::MemoryBlock& dest_data) override;
    void setStateInformation(const void* data, int size_in_bytes) override;

    juce::AudioProcessorValueTreeState tree_state_;
    juce::dsp::Convolution speaker_module_;

   private:
    juce::dsp::ProcessSpec spec_;
    juce::dsp::Gain<float> input_gain_;
    juce::dsp::Gain<float> output_gain_;
    Distortion<float> distortion_;
    Tremolo<float> tremolo_;
    void updateParams();
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void parameterChanged(const juce::String& parameter_id,
                          float new_value) override;

    static constexpr float kCompensationGain = 6.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessor)
};
