#ifndef GUITAR_AMPLIFIER_DISTORTION_H
#define GUITAR_AMPLIFIER_DISTORTION_H

#include <juce_dsp/juce_dsp.h>

template <typename SampleType>
class Distortion {
   public:
    Distortion();

    void prepare(const juce::dsp::ProcessSpec& spec) noexcept;

    void reset() noexcept;

    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept
    {
        auto&& in_block = context.getInputBlock();
        auto&& out_block = context.getOutputBlock();

        jassert (in_block.getNumChannels() == out_block.getNumChannels());
        jassert (in_block.getNumSamples() == out_block.getNumSamples());

        auto len         = in_block.getNumSamples();
        auto num_channels = in_block.getNumChannels();

        for (size_t channel = 0; channel < num_channels; ++channel)
        {
            for (size_t sample = 0; sample < len; ++sample)
            {
                auto* input = in_block.getChannelPointer (channel);
                auto* output = out_block.getChannelPointer (channel);

                output[sample] = processSample(input[sample], channel);
            }
        }
    }

    SampleType processSample(SampleType input, int ch) noexcept {
        auto wet_input = input;

        wet_input *= drive_.getNextValue();
        wet_input = pi_divisor_ * std::atan(wet_input);

        auto mix = (wet_input * mix_.getNextValue()) + (input * (1.0 - mix_.getNextValue()));
        return mix;
    }

    void set_drive(SampleType drive);
    void set_mix(SampleType mix);

   private:
    double sample_rate_;
    juce::SmoothedValue<float> drive_;
    juce::SmoothedValue<float> mix_;

    static constexpr float pi_divisor_ = 2.0f / juce::MathConstants<float>::pi;
};

#endif  // GUITAR_AMPLIFIER_DISTORTION_H
