/*
  ==============================================================================

    wpFFT.cpp
    Created: 21 May 2023 2:44:28am
    Author:  Paxton Fleming
    
    using code from Will Pirkle's Designing Audio Effect Plugins in C++: For AAX, AU, and VST3 with DSP Theory

  ==============================================================================
*/

#include "wpFFT.h"

void FastFFT::destroyFFTW()
	{
		if (plan_forward)
			fftw_destroy_plan(plan_forward);
		if (plan_backward)
			fftw_destroy_plan(plan_backward);

		if (fft_input)
			fftw_free(fft_input);
		if (fft_result)
			fftw_free(fft_result);

		if (ifft_input)
			fftw_free(ifft_input);
		if (ifft_result)
			fftw_free(ifft_result);
	}

void FastFFT::initialize(unsigned int _frameLength, windowType _window)
{
	frameLength = _frameLength;
	window = _window;
	windowGainCorrection = 0.0;

	if (windowBuffer)
		delete windowBuffer;

	windowBuffer = new double[frameLength];
	memset(&windowBuffer[0], 0, frameLength * sizeof(double));


	// --- this is from Reiss & McPherson's code
	//     https://code.soundsoftware.ac.uk/projects/audio_effects_textbook_code/repository/entry/effects/pvoc_passthrough/Source/PluginProcessor.cpp
	// NOTE:	"Window functions are typically defined to be symmetrical. This will cause a
	//			problem in the overlap-add process: the windows instead need to be periodic
	//			when arranged end-to-end. As a result we calculate the window of one sample
	//			larger than usual, and drop the last sample. (This works as long as N is even.)
	//			See Julius Smith, "Spectral Audio Signal Processing" for details.
	// --- WP: this is why denominators are (frameLength) rather than (frameLength - 1)
	if (window == windowType::kRectWindow)
	{
		for (int n = 0; n < frameLength - 1; n++)
		{
			windowBuffer[n] = 1.0;
			windowGainCorrection += windowBuffer[n];
		}
	}
	else if (window == windowType::kHammingWindow)
	{
		for (int n = 0; n < frameLength - 1; n++)
		{
			windowBuffer[n] = 0.54 - 0.46 * cos((n * 2.0 * kPi) / (frameLength));
			windowGainCorrection += windowBuffer[n];
		}
	}
	else if (window == windowType::kHannWindow)
	{
		for (int n = 0; n < frameLength; n++)
		{
			windowBuffer[n] = 0.5 * (1 - cos((n * 2.0 * kPi) / (frameLength)));
			windowGainCorrection += windowBuffer[n];
		}
	}
	else if (window == windowType::kBlackmanHarrisWindow)
	{
		for (int n = 0; n < frameLength; n++)
		{
			windowBuffer[n] = (0.42323 - (0.49755 * cos((n * 2.0 * kPi) / (frameLength))) + 0.07922 * cos((2 * n * 2.0 * kPi) / (frameLength)));
			windowGainCorrection += windowBuffer[n];
		}
	}
	else if (window == windowType::kNoWindow)
	{
		for (int n = 0; n < frameLength; n++)
		{
			windowBuffer[n] = 1.0;
			windowGainCorrection += windowBuffer[n];
		}
	}
	else // --- default to kNoWindow
	{
		for (int n = 0; n < frameLength; n++)
		{
			windowBuffer[n] = 1.0;
			windowGainCorrection += windowBuffer[n];
		}
	}

	// --- calculate gain correction factor
	windowGainCorrection = 1.0 / windowGainCorrection;

	destroyFFTW();
	fft_input = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * frameLength);
	fft_result = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * frameLength);

	ifft_input = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * frameLength);
	ifft_result = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * frameLength);

	plan_forward = fftw_plan_dft_1d(frameLength, fft_input, fft_result, FFTW_FORWARD, FFTW_ESTIMATE);
	plan_backward = fftw_plan_dft_1d(frameLength, ifft_input, ifft_result, FFTW_BACKWARD, FFTW_ESTIMATE);
}

/**
\brief perform the FFT operation

- NOTES:<br>

\param inputReal an array of real valued points
\param inputImag an array of imaginary valued points (will be 0 for audio which is real-valued)

\returns a pointer to a fftw_complex array: a 2D array of real (column 0) and imaginary (column 1) parts
*/
fftw_complex* FastFFT::doFFT(double* inputReal, double* inputImag)
{
	// ------ load up the FFT input array
	for (int i = 0; i < frameLength; i++)
	{
		fft_input[i][0] = inputReal[i];		// --- real
		if (inputImag)
			fft_input[i][1] = inputImag[i]; // --- imag
		else
			fft_input[i][1] = 0.0;
	}

	// --- do the FFT
	fftw_execute(plan_forward);

	return fft_result;
}

/**
\brief perform the IFFT operation

- NOTES:<br>

\param inputReal an array of real valued points
\param inputImag an array of imaginary valued points (will be 0 for audio which is real-valued)

\returns a pointer to a fftw_complex array: a 2D array of real (column 0) and imaginary (column 1) parts
*/
fftw_complex* FastFFT::doInverseFFT(double* inputReal, double* inputImag)
{
	// ------ load up the iFFT input array
	for (int i = 0; i < frameLength; i++)
	{
		ifft_input[i][0] = inputReal[i];		// --- real
		if (inputImag)
			ifft_input[i][1] = inputImag[i]; // --- imag
		else
			ifft_input[i][1] = 0.0;
	}

	// --- do the IFFT
	fftw_execute(plan_backward);

	return ifft_result;
}

/**
\brief destroys the FFTW arrays and plans.
*/
void PhaseVocoder::destroyFFTW()
{
	if (plan_forward)
		fftw_destroy_plan(plan_forward);
	if (plan_backward)
		fftw_destroy_plan(plan_backward);

	if (fft_input)
		fftw_free(fft_input);
	if (fft_result)
		fftw_free(fft_result);
	if (ifft_result)
		fftw_free(ifft_result);
}

/**
\brief initialize the Fast FFT object for operation

- NOTES:<br>
See notes on symmetrical window arrays in comments.<br>

\param _frameLength the FFT length - MUST be a power of 2
\param _hopSize the hop size in samples: this object only supports ha = hs (pure real-time operation only)
\param _window the window type (note: may be set to windowType::kNoWindow)

*/
void PhaseVocoder::initialize(unsigned int _frameLength, unsigned int _hopSize, windowType _window)
{
	frameLength = _frameLength;
	wrapMask = frameLength - 1;
	hopSize = _hopSize;
	window = _window;

	// --- this is the overlap as a fraction i.e. 0.75 = 75%
	overlap = hopSize > 0.0 ? 1.0 - (double)hopSize / (double)frameLength : 0.0;

	// --- gain correction for window + hop size
	windowHopCorrection = 0.0;

	// --- SETUP BUFFERS ---- //
	//     NOTE: input and output buffers are circular, others are linear
	//
	// --- input buffer, for processing the x(n) timeline
	if (inputBuffer)
		delete inputBuffer;

	inputBuffer = new double[frameLength];
	memset(&inputBuffer[0], 0, frameLength * sizeof(double));

	// --- output buffer, for processing the y(n) timeline and accumulating frames
	if (outputBuffer)
		delete outputBuffer;

	// --- the output buffer is declared as 2x the normal frame size
	//     to accomodate time-stretching/pitch shifting; you can increase the size
	//     here; if so make sure to calculate the wrapMaskOut properly and everything
	//     will work normally you can even dynamically expand and contract the buffer
	//     (not sure why you would do this - and it will surely affect CPU performance)
	//     NOTE: the length of the buffer is only to accomodate accumulations
	//           it does not stretch time or change causality on its own
	outputBuffer = new double[frameLength * 4];
	memset(&outputBuffer[0], 0, (frameLength * 4.0) * sizeof(double));
	wrapMaskOut = (frameLength * 4.0) - 1;

	// --- fixed window buffer
	if (windowBuffer)
		delete windowBuffer;

	windowBuffer = new double[frameLength];
	memset(&windowBuffer[0], 0, frameLength * sizeof(double));

	// --- this is from Reiss & McPherson's code
	//     https://code.soundsoftware.ac.uk/projects/audio_effects_textbook_code/repository/entry/effects/pvoc_passthrough/Source/PluginProcessor.cpp
	// NOTE:	"Window functions are typically defined to be symmetrical. This will cause a
	//			problem in the overlap-add process: the windows instead need to be periodic
	//			when arranged end-to-end. As a result we calculate the window of one sample
	//			larger than usual, and drop the last sample. (This works as long as N is even.)
	//			See Julius Smith, "Spectral Audio Signal Processing" for details.
	// --- WP: this is why denominators are (frameLength) rather than (frameLength - 1)
	if (window == windowType::kRectWindow)
	{
		for (int n = 0; n < frameLength - 1; n++)
		{
			windowBuffer[n] = 1.0;
			windowHopCorrection += windowBuffer[n];
		}
	}
	else if (window == windowType::kHammingWindow)
	{
		for (int n = 0; n < frameLength - 1; n++)
		{
			windowBuffer[n] = 0.54 - 0.46 * cos((n * 2.0 * kPi) / (frameLength));
			windowHopCorrection += windowBuffer[n];
		}
	}
	else if (window == windowType::kHannWindow)
	{
		for (int n = 0; n < frameLength; n++)
		{
			windowBuffer[n] = 0.5 * (1 - cos((n * 2.0 * kPi) / (frameLength)));
			windowHopCorrection += windowBuffer[n];
		}
	}
	else if (window == windowType::kBlackmanHarrisWindow)
	{
		for (int n = 0; n < frameLength; n++)
		{
			windowBuffer[n] = (0.42323 - (0.49755 * cos((n * 2.0 * kPi) / (frameLength))) + 0.07922 * cos((2 * n * 2.0 * kPi) / (frameLength)));
			windowHopCorrection += windowBuffer[n];
		}
	}
	else if (window == windowType::kNoWindow)
	{
		for (int n = 0; n < frameLength; n++)
		{
			windowBuffer[n] = 1.0;
			windowHopCorrection += windowBuffer[n];
		}
	}
	else // --- default to kNoWindow
	{
		for (int n = 0; n < frameLength; n++)
		{
			windowBuffer[n] = 1.0;
			windowHopCorrection += windowBuffer[n];
		}
	}

	// --- calculate gain correction factor
	if (window != windowType::kNoWindow)
		windowHopCorrection = (1.0 - overlap) / windowHopCorrection;
	else
		windowHopCorrection = 1.0 / windowHopCorrection;

	// --- set
	inputWriteIndex = 0;
	inputReadIndex = 0;

	outputWriteIndex = 0;
	outputReadIndex = 0;

	fftCounter = 0;

	// --- reset flags
	needInverseFFT = false;
	needOverlapAdd = false;


	destroyFFTW();
	fft_input = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * frameLength);
	fft_result = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * frameLength);
	ifft_result = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * frameLength);

	plan_forward = fftw_plan_dft_1d(frameLength, fft_input, fft_result, FFTW_FORWARD, FFTW_ESTIMATE);
	plan_backward = fftw_plan_dft_1d(frameLength, fft_result, ifft_result, FFTW_BACKWARD, FFTW_ESTIMATE);
}


/**
\brief zero pad the input timeline

- NOTES:<br>

\param count the number of zero-valued samples to insert

\returns true if the zero-insertion triggered a FFT event, false otherwise
*/
bool PhaseVocoder::addZeroPad(unsigned int count)
	{
		bool fftReady = false;
		for (unsigned int i = 0; i < count; i++)
		{
			// --- push into buffer
			inputBuffer[inputWriteIndex++] = 0.0;

			// --- wrap
			inputWriteIndex &= wrapMask;

			// --- check the FFT
			bool didFFT = advanceAndCheckFFT();

			// --- for a zero-padding operation, the last inserted zero
			//     should trigger the FFT; if not something has gone horribly wrong
			if (didFFT && i == count - 1)
				fftReady = true;
		}

		return fftReady;
	}

	/**
	\brief advance the sample counter and check to see if we need to do the FFT.

	- NOTES:<br>

	\returns true if the advancement triggered a FFT event, false otherwise
	*/
	bool PhaseVocoder::advanceAndCheckFFT()
	{
		// --- inc counter and check count
		fftCounter++;

		if (fftCounter != frameLength)
			return false;

		// --- we have a FFT ready
		// --- load up the input to the FFT
		for (int i = 0; i < frameLength; i++)
		{
			fft_input[i][0] = inputBuffer[inputReadIndex++] * windowBuffer[i];
			fft_input[i][1] = 0.0; // use this if your data is complex valued

			// --- wrap if index > bufferlength - 1
			inputReadIndex &= wrapMask;
		}

		// --- do the FFT
		fftw_execute(plan_forward);

		// --- in case user does not take IFFT, just to prevent zero output
		needInverseFFT = true;
		needOverlapAdd = true;

		// --- fft counter: small hop = more FFTs = less counting before fft
		//
		// --- overlap-add-only algorithms do not involve hop-size in FFT count
		if (overlapAddOnly)
			fftCounter = 0;
		else // normal counter advance
			fftCounter = frameLength - hopSize;

		// --- setup the read index for next time through the loop
		if (!overlapAddOnly)
			inputReadIndex += hopSize;

		// --- wrap if needed
		inputReadIndex &= wrapMask;

		return true;
	}

	/**
	\brief process one input sample throug the vocoder to produce one output sample

	- NOTES:<br>

	\param input the input sample x(n)
	\param fftReady a return flag indicating if the FFT has occurred and FFT data is ready to process

	\returns the vocoder output sample y(n)
	*/
	double PhaseVocoder::processAudioSample(double input, bool& fftReady)
	{
		// --- if user did not manually do fft and overlap, do them here
		//     this allows maximum flexibility in use of the object
		if (needInverseFFT)
			doInverseFFT();
		if (needOverlapAdd)
			doOverlapAdd();

		fftReady = false;

		// --- get the current output sample first
		double currentOutput = outputBuffer[outputReadIndex];

		// --- set the buffer to 0.0 in preparation for the next overlap/add process
		outputBuffer[outputReadIndex++] = 0.0;

		// --- wrap
		outputReadIndex &= wrapMaskOut;

		// --- push into buffer
		inputBuffer[inputWriteIndex++] = (double)input;

		// --- wrap
		inputWriteIndex &= wrapMask;

		// --- check the FFT
		fftReady = advanceAndCheckFFT();

		return currentOutput;
	}

	/**
	\brief perform the inverse FFT on the processed data

	- NOTES:<br>
	This function is optional - if you need to sequence the output (synthesis) stage yourself <br>
	then you can call this function at the appropriate time - see the PSMVocoder object for an example

	*/
	void PhaseVocoder::doInverseFFT()
	{
		// do the IFFT
		fftw_execute(plan_backward);

		// --- output is now in ifft_result array
		needInverseFFT = false;
	}

	/**
	\brief perform the overlap/add on the IFFT data

	- NOTES:<br>
	This function is optional - if you need to sequence the output (synthesis) stage yourself <br>
	then you can call this function at the appropriate time - see the PSMVocoder object for an example

	\param outputData an array of data to overlap/add: if this is NULL then the IFFT data is used
	\param length the lenght of the array of data to overlap/add: if this is -1, the normal IFFT length is used
	*/
	void PhaseVocoder::doOverlapAdd(double* outputData, int length)
	{
		// --- overlap/add with output buffer
		//     NOTE: this assumes input and output hop sizes are the same!
		outputWriteIndex = outputReadIndex;

		if (outputData)
		{
			for (int i = 0; i < length; i++)
			{
				// --- if you need to window the data, do so prior to this function call
				outputBuffer[outputWriteIndex++] += outputData[i];

				// --- wrap if index > bufferlength - 1
				outputWriteIndex &= wrapMaskOut;
			}
			needOverlapAdd = false;
			return;
		}

		for (int i = 0; i < frameLength; i++)
		{
			// --- accumulate
			outputBuffer[outputWriteIndex++] += windowHopCorrection * ifft_result[i][0];

			// --- wrap if index > bufferlength - 1
			outputWriteIndex &= wrapMaskOut;
		}

		// --- set a flag
		needOverlapAdd = false;
	}
