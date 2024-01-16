#include "distortion.h"

template <typename SampleType>
Distortion<SampleType>::Distortion() {}

template <typename SampleType>
void Distortion<SampleType>::prepare(
    const juce::dsp::ProcessSpec& spec) noexcept {
    sample_rate_ = spec.sampleRate;
    reset();
}

template <typename SampleType>
void Distortion<SampleType>::reset() noexcept {
    drive_.reset(sample_rate_, 0.02);
    mix_.reset(sample_rate_, 0.02);
}

template <typename SampleType>
void Distortion<SampleType>::set_drive(SampleType drive) {
    drive_.setTargetValue(drive);
}

template <typename SampleType>
void Distortion<SampleType>::set_mix(SampleType mix) {
    mix_.setTargetValue(mix);
}

template class Distortion<float>;
template class Distortion<double>;
