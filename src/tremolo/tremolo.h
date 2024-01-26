#ifndef GUITAR_AMPLIFIER_TREMOLO_H
#define GUITAR_AMPLIFIER_TREMOLO_H

#include <juce_dsp/juce_dsp.h>

template <typename SampleType>
class Tremolo {
   public:
    Tremolo();

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
            index_ -= len;
        }
        index_ += len;
    }

    SampleType processSample(SampleType input, int ch) noexcept {
        float depth = depth_.getNextValue();
        float output = input * (1.0f - depth) + input * depth * std::sin(index_ * 2.0f * juce::MathConstants<float>::pi / sample_rate_);

        index_ = (index_ + 1) % (int)(sample_rate_ / rate_.getNextValue());
        return output;
    }

    void set_rate(SampleType rate);
    void set_depth(SampleType depth);

   private:
    double sample_rate_;
    int index_;
    juce::SmoothedValue<float> rate_;
    juce::SmoothedValue<float> depth_;
};

#endif  // GUITAR_AMPLIFIER_TREMOLO_H
