/*
  ==============================================================================

    FFTEffect.h
    Created: 29 Sep 2023 7:34:45pm
    Author:  Paxton Fleming

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "wpFFT.h"
#include "sincostable.h"

struct BinData
{
    BinData() {}
    /** reset all variables to 0.0 */
    void reset()
    {
        isPeak = false;
        magnitude = 0.0;
        phi = 0.0;

        psi = 0.0;
        localPeakBin = 0;
        previousPeakBin = -1; // -1 is flag
        updatedPhase = 0.0;
    }

    bool isPeak = false;	///< flag for peak bins
    double magnitude = 0.0; ///< bin magnitude angle
    double phi = 0.0;		///< bin phase angle
    double psi = 0.0;		///< bin phase correction
    unsigned int localPeakBin = 0; ///< index of peak-boss
    int previousPeakBin = -1; ///< index of peak bin in previous FFT
    double updatedPhase = 0.0; ///< phase update value
};

class FFTEffect
{
public:

    FFTEffect();

    int findPreviousNearestPeak(int peakIndex);
    void findPeaksAndRegionsOfInfluence();

    void scramble(fftw_complex* complexData, int dataSize, double width);
    void smear(fftw_complex* complexData, int dataSize, double width);
    void contrast(fftw_complex* complexData, int dataSize, float raiseVal);

 
    void setFxMode(FxMode inFxMode);
    void setFxValue(float newValue);

    void setPitchShift(double semitones);
    double process(double sample);



private:
    SinCosTable<12> g_sincos_table;
    std::vector<double> newMagnitudes;
    PhaseVocoder phaseVocoder;
    FxMode m_fxMode = FxMode::scramble;

    float m_scramblingWidth = 0.2f;
    float m_smearingWidth = 0.2f;
    float m_contrastValue = 0.2f;

    bool enablePeakPhaseLocking = true;
    bool enablePeakTracking = true;
    double alphaStretchRatio = 1.0;

    const double hs = FFT_LEN / 4;	///< hs = N/4 --- 75% overlap
	double ha = FFT_LEN / 4;		///< ha = N/4 --- 75% overlap
	double phi[FFT_LEN] = { 0.0 };	///< array of phase values for classic algorithm
	double psi[FFT_LEN] = { 0.0 };	///< array of phase correction values for classic algorithm

	// --- for peak-locking
	BinData binData[FFT_LEN];			///< array of BinData structures for current FFT frame
	BinData binDataPrevious[FFT_LEN];	///< array of BinData structures for previous FFT frame

	int peakBins[FFT_LEN] = { -1 };		///< array of current peak bin index values (-1 = not peak)
	int peakBinsPrevious[FFT_LEN] = { -1 }; ///< array of previous peak bin index values (-1 = not peak)

    std::vector<double> windowBuff;
    std::vector<double> outputBuff;
    double windowCorrection;
    unsigned int outputBufferLength;
};