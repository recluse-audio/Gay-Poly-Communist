/*
  ==============================================================================

    GayFilter.cpp
    Created: 22 Aug 2022 6:39:37pm
    Author:  Ryan Devens

  ==============================================================================
*/

#include "GayFilter.h"
#include "GayOscillator.h"
#include "GayADSR.h"


//====================================
GayFilter::GayFilter()
{
	
}

//====================================
GayFilter::~GayFilter()
{
	
}

//====================================
void GayFilter::prepare(const juce::dsp::ProcessSpec& spec)
{
	filter.prepare(spec);
	
	filtFreq.reset			(spec.sampleRate, 0.01);
	filtFreq_LFODepth.reset (spec.sampleRate, 0.01);
	filtFreq_EnvDepth.reset (spec.sampleRate, 0.01);
	
	filtDrive.reset			(spec.sampleRate, 0.01);
	filtDrive_LFODepth.reset(spec.sampleRate, 0.01);
	filtDrive_EnvDepth.reset(spec.sampleRate, 0.01);
	
	filtReson.reset			(spec.sampleRate, 0.01);
	filtReson_LFODepth.reset(spec.sampleRate, 0.01);
	filtReson_EnvDepth.reset(spec.sampleRate, 0.01);
	
}

//====================================
void GayFilter::update(juce::AudioProcessorValueTreeState& apvts)
{
	
}

//====================================
void GayFilter::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
	
}

//====================================
void GayFilter::assignResonLFO(GayOscillator *lfo)
{
	
}

//====================================
void GayFilter::assignFreqLFO(GayOscillator *lfo)
{
	
}

//====================================
void GayFilter::assignDriveLFO(GayOscillator *lfo)
{
	
}

//====================================
void GayFilter::assignResonEnvelope(GayADSR *env)
{
	
}

//====================================
void GayFilter::assignFreqEnvelope(GayADSR *env)
{
	
}

//====================================
void GayFilter::assignDriveEnvelope(GayADSR *env)
{
	
}

//====================================
void GayFilter::incrementParameters()
{
	_incrementParameters();
}


//***************************
// PRIVATE FUNCTIONS

//====================================
void GayFilter::_incrementParameters()
{
	
}

//====================================
void GayFilter::_applyFreqModulation()
{
	
}

//====================================
void GayFilter::_applyDriveModulation()
{
	
}

//====================================
void GayFilter::_applyResonModulation()
{
	
}
