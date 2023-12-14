/*
  ==============================================================================

    MathConstants.h
    Created: 21 May 2023 2:45:34am
    Author:  Paxton Fleming

  ==============================================================================
*/

#pragma once

#include <vector>
#include <complex>
#include <random>
#include "fftw3.h"


const double kTwoPi = 6.283185307179586;
const double kPi = 3.141592653589793;
const unsigned int FFT_LEN = 4096;
const std::uniform_real_distribution<> kDistribution(0, 2 * kPi);

enum class windowType {
	kNoWindow,
	kRectWindow,
	kHannWindow,
	kBlackmanHarrisWindow,
	kHammingWindow
};

enum class interpolation { kLinear, kLagrange4 };

enum class FxMode { scramble, smear, contrast };

inline float lin_interp(const float& frac, const float& out_min, const float& out_max)
{
	return out_min + (out_max - out_min) * frac;
}


inline double matchPower(float amp, double target_pwr, double curr_pwr)
{
	double s = amp;
	if (target_pwr <= 0)
		return 0;
	if (curr_pwr > 0)
		s *= sqrt(target_pwr / curr_pwr);

	if (s > 1e30)
		return 1.0f;
	if (s < 1e-37)
		return 0.0f;
	return (double)s;
}


inline void matchPower(float amp, double target_pwr, double curr_pwr, fftw_complex* complexData, int i)
{
	amp = matchPower(amp, target_pwr, curr_pwr);
	complexData[i][0] *= amp;
	complexData[i][1] *= amp;

}

inline double getMagnitude(double re, double im)
{
	return sqrt((re * re) + (im * im));
}

inline double getPhase(double re, double im)
{
	return atan2(im, re);
}

inline double principalArg(double phaseIn)
{
	if (phaseIn >= 0)
		return fmod(phaseIn + kPi, kTwoPi) - kPi;
	else
		return fmod(phaseIn + kPi, -kTwoPi) + kPi;
}

inline double doLinearInterpolation(double y1, double y2, double fractional_X)
{
	// --- check invalid condition
	if (fractional_X >= 1.0) return y2;

	// --- use weighted sum method of interpolating
	return fractional_X * y2 + (1.0 - fractional_X) * y1;
}

inline double doLagrangeInterpolation(double* x, double* y, int n, double xbar)
{
	int i, j;
	double fx = 0.0;
	double l = 1.0;
	for (i = 0; i < n; i++)
	{
		l = 1.0;
		for (j = 0; j < n; j++)
		{
			if (j != i)
				l *= (xbar - x[j]) / (x[i] - x[j]);
		}
		fx += l * y[i];
	}
	return (fx);
}

inline bool resample(double* input, double* output, uint32_t inLength, uint32_t outLength,
	interpolation interpType = interpolation::kLinear,
	double scalar = 1.0, double* outWindow = nullptr)
{
	if (inLength <= 1 || outLength <= 1) return false;
	if (!input || !output) return false;

	double x[4] = { 0.0, 0.0, 0.0, 0.0 };
	double y[4] = { 0.0, 0.0, 0.0, 0.0 };

	// --- inc
	double inc = (double)(inLength - 1) / (double)(outLength - 1);

	// --- first point
	if (outWindow)
		output[0] = outWindow[0] * scalar * input[0];
	else
		output[0] = scalar * input[0];

	if (interpType == interpolation::kLagrange4)
	{
		for (unsigned int i = 1; i < outLength; i++)
		{
			// --- find interpolation location
			double xInterp = i * inc;
			uint32_t x1 = (uint32_t)xInterp; // floor?

			if (xInterp > 1.0 && x1 < inLength - 2)
			{
				x[0] = x1 - 1;
				y[0] = input[(int)x[0]];

				x[1] = x1;
				y[1] = input[(int)x[1]];

				x[2] = x1 + 1;
				y[2] = input[(int)x[2]];

				x[3] = x1 + 2;
				y[3] = input[(int)x[3]];

				if (outWindow)
					output[i] = outWindow[i] * scalar * doLagrangeInterpolation(x, y, 4, xInterp);
				else
					output[i] = scalar * doLagrangeInterpolation(x, y, 4, xInterp);
			}
			else // --- linear for outer 2 end pts
			{
				uint32_t x2 = x1 + 1;
				if (x2 >= outLength)
					x2 = x1;
				double y1 = input[x1];
				double y2 = input[x2];

				if (outWindow)
					output[i] = outWindow[i] * scalar * doLinearInterpolation(y1, y2, xInterp - x1);
				else
					output[i] = scalar * doLinearInterpolation(y1, y2, xInterp - x1);
			}
		}
	}
	else // must be linear
	{
		// --- LINEAR INTERP
		for (uint32_t i = 1; i < outLength; i++)
		{
			double xInterp = i * inc;
			uint32_t x1 = (uint32_t)xInterp; // floor?
			uint32_t x2 = x1 + 1;
			if (x2 >= outLength)
				x2 = x1;
			double y1 = input[x1];
			double y2 = input[x2];

			if (outWindow)
				output[i] = outWindow[i] * scalar * doLinearInterpolation(y1, y2, xInterp - x1);
			else
				output[i] = scalar * doLinearInterpolation(y1, y2, xInterp - x1);
		}
	}

	return true;
}



inline double computeNorm(fftw_complex c) {
	return c[0] * c[0] + c[1] * c[1];
}

inline std::complex<double> toStdComplex(fftw_complex c) {
	return std::complex<double>(c[0], c[1]);
}

inline void fromStdComplex(std::complex<double> src, fftw_complex& dst) {
	dst[0] = src.real();
	dst[1] = src.imag();
}

inline static std::mt19937 getRandom()
{
	std::random_device rd;
	static std::mt19937 g(rd());
	return g;
}

inline long prbs32(long x)
{
	return ((unsigned long)x >> 1) ^ (-(x & 1) & 0xd0000001UL);
}
