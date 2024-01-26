#include "plugin_processor.h"

#include "plugin_editor.h"

AudioPluginAudioProcessor::AudioPluginAudioProcessor()
    : AudioProcessor(
          BusesProperties()
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      tree_state_(*this, nullptr, "PARAMETERS", createParameterLayout()) {
    tree_state_.addParameterListener(kInputId, this);
    tree_state_.addParameterListener(kOutputId, this);
    tree_state_.addParameterListener(kDistortionDriveId, this);
    tree_state_.addParameterListener(kDistortionMixId, this);
    tree_state_.addParameterListener(kTremoloDepthId, this);
    tree_state_.addParameterListener(kTremoloRateId, this);
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor() {
    tree_state_.removeParameterListener(kInputId, this);
    tree_state_.removeParameterListener(kOutputId, this);
    tree_state_.removeParameterListener(kDistortionDriveId, this);
    tree_state_.removeParameterListener(kDistortionMixId, this);
    tree_state_.removeParameterListener(kTremoloDepthId, this);
    tree_state_.removeParameterListener(kTremoloRateId, this);
}

juce::AudioProcessorValueTreeState::ParameterLayout
AudioPluginAudioProcessor::createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        kDistortionDriveId, kDistortionDriveName, 0.0f, 100.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        kDistortionMixId, kDistortionMixName, 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        kInputId, kInputName, -24.0f, 24.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        kTremoloDepthId, kTremoloDepthName, 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        kTremoloRateId, kTremoloRateName, 1.0f, 10.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        kOutputId, kOutputName, -24.0f, 24.0f, 0.0f));

    return {params.begin(), params.end()};
}

void AudioPluginAudioProcessor::parameterChanged(
    const juce::String& parameter_id, float new_value) {
    updateParams();
}

void AudioPluginAudioProcessor::updateParams() {
    distortion_.set_drive(
        tree_state_.getRawParameterValue(kDistortionDriveId)->load());
    distortion_.set_mix(
        tree_state_.getRawParameterValue(kDistortionMixId)->load());

    input_gain_.setGainDecibels(
        tree_state_.getRawParameterValue(kInputId)->load());
    output_gain_.setGainDecibels(
        tree_state_.getRawParameterValue(kOutputId)->load() +
        kCompensationGain);

    tremolo_.set_depth(
        tree_state_.getRawParameterValue(kTremoloDepthId)->load());
    tremolo_.set_rate(
        tree_state_.getRawParameterValue(kTremoloRateId)->load());
}

const juce::String AudioPluginAudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int AudioPluginAudioProcessor::getNumPrograms() {
    return 1;  // NB: some hosts don't cope very well if you tell them there are
               // 0 programs, so this should be at least 1, even if you're not
               // really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram() { return 0; }

void AudioPluginAudioProcessor::setCurrentProgram(int index) {
    juce::ignoreUnused(index);
}

const juce::String AudioPluginAudioProcessor::getProgramName(int index) {
    juce::ignoreUnused(index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName(
    int index, const juce::String& new_name) {
    juce::ignoreUnused(index, new_name);
}

void AudioPluginAudioProcessor::prepareToPlay(double sample_rate,
                                              int samples_per_block) {
    spec_.sampleRate = sample_rate;
    spec_.maximumBlockSize = samples_per_block;
    spec_.numChannels = getTotalNumOutputChannels();

    speaker_module_.prepare(spec_);
    speaker_module_.loadImpulseResponse(
        BinaryData::ir_wav, BinaryData::ir_wavSize,
        juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::yes,
        0, juce::dsp::Convolution::Normalise::yes);

    input_gain_.prepare(spec_);
    input_gain_.setRampDurationSeconds(0.02);

    output_gain_.prepare(spec_);
    output_gain_.setRampDurationSeconds(0.02);

    distortion_.prepare(spec_);
    distortion_.set_drive(0.0);
    distortion_.set_mix(0.5);

    tremolo_.prepare(spec_);
    tremolo_.set_depth(0.0);
    tremolo_.set_rate(1.0);

    updateParams();
}

void AudioPluginAudioProcessor::releaseResources() {
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer& midi_messages) {
    juce::ignoreUnused(midi_messages);

    juce::ScopedNoDenormals no_denormals;

    juce::dsp::AudioBlock<float> block{buffer};
    distortion_.process(juce::dsp::ProcessContextReplacing<float>(block));
    input_gain_.process(juce::dsp::ProcessContextReplacing<float>(block));
    tremolo_.process(juce::dsp::ProcessContextReplacing<float>(block));
    speaker_module_.process(juce::dsp::ProcessContextReplacing<float>(block));
    output_gain_.process(juce::dsp::ProcessContextReplacing<float>(block));
}

bool AudioPluginAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor() {
    return new juce::GenericAudioProcessorEditor(*this);
}

void AudioPluginAudioProcessor::getStateInformation(
    juce::MemoryBlock& dest_data) {
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused(dest_data);
}

void AudioPluginAudioProcessor::setStateInformation(const void* data,
                                                    int size_in_bytes) {
    // You should use this method to restore your parameters from this memory
    // block, whose contents will have been created by the getStateInformation()
    // call.
    juce::ignoreUnused(data, size_in_bytes);
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new AudioPluginAudioProcessor();
}
