/*
  ==============================================================================

    GayFilter.h
    Created: 22 Aug 2022 6:39:37pm
    Author:  Ryan Devens

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class GayOscillator;
class GayADSR;
/*
	Interface class for handling a Ladder Filter
	Stores/sets params
	Handles value smoothing over time
*/
class GayFilter
{
public:
	GayFilter();
	~GayFilter();
	void prepare(const juce::dsp::ProcessSpec& spec);
	void update(juce::AudioProcessorValueTreeState& apvts);
	void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples);
	
	void assignResonLFO	(GayOscillator* lfo);
	void assignFreqLFO	(GayOscillator* lfo);
	void assignDriveLFO	(GayOscillator* lfo);
	
	void assignResonEnvelope	(GayADSR* env);
	void assignFreqEnvelope		(GayADSR* env);
	void assignDriveEnvelope	(GayADSR* env);

	void incrementParameters();
private:
	dsp::LadderFilter<float> filter;
	bool isFiltering = true;
	
	juce::SmoothedValue<float> filtFreq;
	juce::SmoothedValue<float> filtFreq_LFODepth;
	juce::SmoothedValue<float> filtFreq_EnvDepth;
	
	juce::SmoothedValue<float> filtDrive;
	juce::SmoothedValue<float> filtDrive_LFODepth;
	juce::SmoothedValue<float> filtDrive_EnvDepth;
	
	juce::SmoothedValue<float> filtReson;
	juce::SmoothedValue<float> filtReson_LFODepth;
	juce::SmoothedValue<float> filtReson_EnvDepth;
	
	// Oscillators modulating parameters of this oscillator
	GayOscillator* resonLFO;
	GayOscillator* freqLFO;
	GayOscillator* driveLFO;

	// Envelopes modulating parameters of this oscillator
	GayADSR* resonEnv;
	GayADSR* freqEnv;
	GayADSR* driveEnv;

	
	void _incrementParameters();
	
	// Applies current modulations and offsets to the given parameter
	void _applyResonModulation();
	void _applyFreqModulation();
	void _applyDriveModulation();
};
