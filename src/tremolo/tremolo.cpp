#include "tremolo.h"

template <typename SampleType>
Tremolo<SampleType>::Tremolo() {}

template <typename SampleType>
void Tremolo<SampleType>::prepare(
    const juce::dsp::ProcessSpec& spec) noexcept {
    sample_rate_ = spec.sampleRate;
    index_ = 0;
    reset();
}

template <typename SampleType>
void Tremolo<SampleType>::reset() noexcept {
    rate_.reset(sample_rate_, 0.02);
    depth_.reset(sample_rate_, 0.02);
}

template <typename SampleType>
void Tremolo<SampleType>::set_rate(SampleType rate) {
    rate_.setTargetValue(rate);
}

template <typename SampleType>
void Tremolo<SampleType>::set_depth(SampleType depth) {
    depth_.setTargetValue(depth);
}

template class Tremolo<float>;
template class Tremolo<double>;