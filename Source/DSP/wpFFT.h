/*
  ==============================================================================

    wpFFT.h
    Created: 21 May 2023 2:42:45am
    Author:  Paxton Fleming
    
    using code from Will Pirkle's Designing Audio Effect Plugins in C++: For AAX, AU, and VST3 with DSP Theory

  ==============================================================================
*/

#pragma once

#include <math.h>
#include <memory>
#include "MathConstants.h"
#include "fftw3.h"

class DFT
{

public:

		DFT();
		~DFT();
	
	void cosineDFT() {
			const int DFTLEN = 128;

			double x[DFTLEN];
			double y[DFTLEN];
			double z[DFTLEN];

			double numCycles = 3;
			
			for (int i = 0; i < DFTLEN; i++)
			{
				x[i] = cos(numCycles * (kTwoPi * (double)i) / DFTLEN);
			}

			for (int BIN = 0; BIN < DFTLEN; BIN++)
			{
				for (int n = 0; n < DFTLEN; n++)
				{
					z[BIN] = z[BIN] + x[n] * cos (kTwoPi * BIN * n / DFTLEN) / DFTLEN;
					y[BIN] = y[BIN] + x[n] * sin (kTwoPi * BIN * n / DFTLEN) / DFTLEN;
				}
			}
	}

};

class FastFFT
{
public:
	FastFFT() {}		/* C-TOR */
	~FastFFT() {
		if (windowBuffer) delete[] windowBuffer;
		destroyFFTW();
	}	/* D-TOR */

	/** setup the FFT for a given framelength and window type*/
	void initialize(unsigned int _frameLength, windowType _window);

	/** destroy FFTW objects and plans */
	void destroyFFTW();

	/** do the FFT and return real and imaginary arrays */
	fftw_complex* doFFT(double* inputReal, double* inputImag = nullptr);

	/** do the IFFT and return real and imaginary arrays */
	fftw_complex* doInverseFFT(double* inputReal, double* inputImag);

	/** get the current FFT length */
	unsigned int getFrameLength() { return frameLength; }

protected:
	// --- setup FFTW
	fftw_complex* fft_input = nullptr;		///< array for FFT input
	fftw_complex* fft_result = nullptr;		///< array for FFT output
	fftw_complex* ifft_input = nullptr;		///< array for IFFT input
	fftw_complex* ifft_result = nullptr;		///< array for IFFT output
	fftw_plan       plan_forward = nullptr;		///< FFTW plan for FFT
	fftw_plan		plan_backward = nullptr;	///< FFTW plan for IFFT

	double* windowBuffer = nullptr;				///< buffer for window (naked)
	double windowGainCorrection = 1.0;			///< window gain correction
	windowType window = windowType::kHannWindow; ///< window type
	unsigned int frameLength = 0;				///< current FFT length
};

class PhaseVocoder
{
public:
	PhaseVocoder() {}		/* C-TOR */
	~PhaseVocoder() {
		if (inputBuffer) delete[] inputBuffer;
		if (outputBuffer) delete[] outputBuffer;
		if (windowBuffer) delete[] windowBuffer;
		destroyFFTW();
	}	/* D-TOR */

	/** setup the FFT for a given framelength and window type*/
	void initialize(unsigned int _frameLength, unsigned int _hopSize, windowType _window);

	/** destroy FFTW objects and plans */
	void destroyFFTW();

	/** process audio sample through vocode; check fftReady flag to access FFT output */
	double processAudioSample(double input, bool& fftReady);

	/** add zero-padding without advancing output read location, for fast convolution */
	bool addZeroPad(unsigned int count);

	/** increment the FFT counter and do the FFT if it is ready */
	bool advanceAndCheckFFT();

	/** get FFT data for manipulation (yes, naked pointer so you can manipulate) */
	fftw_complex* getFFTData() { return fft_result; }

	/** get IFFT data for manipulation (yes, naked pointer so you can manipulate) */
	fftw_complex* getIFFTData() { return ifft_result; }

	/** do the inverse FFT (optional; will be called automatically if not used) */
	void doInverseFFT();

	/** do the overlap-add operation */
	void doOverlapAdd(double* outputData = nullptr, int length = 0);

	/** get current FFT length */
	unsigned int getFrameLength() { return frameLength; }

	/** get current hop size ha = hs */
	unsigned int getHopSize() { return hopSize; }

	/** get current overlap as a raw value (75% = 0.75) */
	double getOverlap() { return overlap; }

	/** set the vocoder for overlap add only without hop-size */
	// --- for fast convolution and other overlap-add algorithms
	//     that are not hop-size dependent
	void setOverlapAddOnly(bool b) { bool overlapAddOnly = b; }

protected:
	// --- setup FFTW
	fftw_complex* fft_input = nullptr;		///< array for FFT input
	fftw_complex* fft_result = nullptr;		///< array for FFT output
	fftw_complex* ifft_result = nullptr;		///< array for IFFT output
	fftw_plan       plan_forward = nullptr;		///< FFTW plan for FFT
	fftw_plan		plan_backward = nullptr;	///< FFTW plan for IFFT

	// --- linear buffer for window
	double* windowBuffer = nullptr;		///< array for window

	// --- circular buffers for input and output
	double* inputBuffer = nullptr;		///< input timeline (x)
	double* outputBuffer = nullptr;		///< output timeline (y)

	// --- index and wrap masks for input and output buffers
	unsigned int inputWriteIndex = 0;			///< circular buffer index: input write
	unsigned int outputWriteIndex = 0;			///< circular buffer index: output write
	unsigned int inputReadIndex = 0;			///< circular buffer index: input read
	unsigned int outputReadIndex = 0;			///< circular buffer index: output read
	unsigned int wrapMask = 0;					///< input wrap mask
	unsigned int wrapMaskOut = 0;				///< output wrap mask

	// --- amplitude correction factor, aking into account both hopsize (overlap)
	//     and the window power itself
	double windowHopCorrection = 1.0;			///< window correction including hop/overlap

	// --- these allow a more robust combination of user interaction
	bool needInverseFFT = false;				///< internal flag to signal IFFT required
	bool needOverlapAdd = false;				///< internal flag to signal overlap/add required

	// --- our window type; you can add more windows if you like
	windowType window = windowType::kHannWindow;///< window type

	// --- counters
	unsigned int frameLength = 0;				///< current FFT length
	unsigned int fftCounter = 0;				///< FFT sample counter

	// --- hop-size and overlap (mathematically related)
	unsigned int hopSize = 0;					///< hop: ha = hs
	double overlap = 1.0;						///< overlap as raw value (75% = 0.75)

	// --- flag for overlap-add algorithms that do not involve hop-size, other
	//     than setting the overlap
	bool overlapAddOnly = false;				///< flag for overlap-add-only algorithms

};