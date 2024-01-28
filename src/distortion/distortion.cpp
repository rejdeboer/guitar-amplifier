#include "distortion.h"

template <typename SampleType>
Distortion<SampleType>::Distortion() : high_pass_filter_() {}

template <typename SampleType>
void Distortion<SampleType>::prepare(
    const juce::dsp::ProcessSpec& spec) noexcept {
    sample_rate_ = spec.sampleRate;
    high_pass_filter_.prepare(spec);
    high_pass_filter_.coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(sample_rate_, 500.0f,
                                                          0.1f);
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
