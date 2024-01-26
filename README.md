# Guitar Amplifier

This project is a guitar amplifier made using the JUCE framework. It is a work in progress.

It currently supports the following features:

- Impulse response simulation
- Very simple distortion
- Simple tremolo effect

## Planned Features

- More distortion types (overdrive, fuzz, etc.)
- Using custom impulse responses
- Reverb
- Delay
- 3-band EQ

## Building

To build this project, you will need to use CMake. The following command will build the project:

```bash
cmake --build Builds --target <target>
```

Where `<target>` is one of the following:

- GuitarAmplifier_Standalone - Builds the standalone application
- GuitarAmplifier_AU - Builds the Audio Unit plugin
- GuitarAmplifier_VST3 - Builds the VST plugin
