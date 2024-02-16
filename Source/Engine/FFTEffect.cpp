/*
  ==============================================================================

    FFTEffect.cpp
    Created: 21 Nov 2023 10:45:42pm
    Author:  Paxton Fleming

  ==============================================================================
*/

#include "FFTEffect.h"

FFTEffect::FFTEffect()
{
    phaseVocoder.initialize(FFT_LEN, FFT_LEN / 4, windowType::kHannWindow);  // 75% overlap
    binData->reset();
    setPitchShift(0);
    setFxMode(FxMode::scramble);

    newMagnitudes.resize(FFT_LEN, 0.0);
}

int FFTEffect::findPreviousNearestPeak(int peakIndex)
{
    if (peakBinsPrevious[0] == -1) // first run, there is no peak
        return -1;

    int delta = -1;
    int previousPeak = -1;
    for (uint32_t i = 0; i < FFT_LEN; i++)
    {
        if (peakBinsPrevious[i] < 0)
            break;

        int dist = abs(peakIndex - peakBinsPrevious[i]);
        if (dist > FFT_LEN / 4)
            break;

        if (i == 0)
        {
            previousPeak = i;
            delta = dist;
        }
        else if (dist < delta)
        {
            previousPeak = i;
            delta = dist;
        }
    }

    return previousPeak;
}

void FFTEffect::findPeaksAndRegionsOfInfluence()
{
    /** identify peak bins and tag their respective regions of influence */


    // --- find local maxima in 4-sample window
    double localWindow[4] = { 0.0, 0.0, 0.0, 0.0 };
    int m = 0;
    for (uint32_t i = 0; i < FFT_LEN; i++)
    {
        if (i == 0)
        {
            localWindow[0] = 0.0;
            localWindow[1] = 0.0;
            localWindow[2] = binData[i + 1].magnitude;
            localWindow[3] = binData[i + 2].magnitude;
        }
        else  if (i == 1)
        {
            localWindow[0] = 0.0;
            localWindow[1] = binData[i - 1].magnitude;
            localWindow[2] = binData[i + 1].magnitude;
            localWindow[3] = binData[i + 2].magnitude;
        }
        else  if (i == FFT_LEN - 1)
        {
            localWindow[0] = binData[i - 2].magnitude;
            localWindow[1] = binData[i - 1].magnitude;
            localWindow[2] = 0.0;
            localWindow[3] = 0.0;
        }
        else  if (i == FFT_LEN - 2)
        {
            localWindow[0] = binData[i - 2].magnitude;
            localWindow[1] = binData[i - 1].magnitude;
            localWindow[2] = binData[i + 1].magnitude;
            localWindow[3] = 0.0;
        }
        else
        {
            localWindow[0] = binData[i - 2].magnitude;
            localWindow[1] = binData[i - 1].magnitude;
            localWindow[2] = binData[i + 1].magnitude;
            localWindow[3] = binData[i + 2].magnitude;
        }

        // --- found peak bin!
        if (binData[i].magnitude > 0.00001 &&
            binData[i].magnitude > localWindow[0]
            && binData[i].magnitude > localWindow[1]
            && binData[i].magnitude > localWindow[2]
            && binData[i].magnitude > localWindow[3])
        {
            binData[i].isPeak = true;
            peakBins[m++] = i;

            // --- for peak bins, assume that it is part of a previous, moving peak
            if (enablePeakTracking)
                binData[i].previousPeakBin = findPreviousNearestPeak(i);
            else
                binData[i].previousPeakBin = -1;
        }
    }

    // --- assign peak bosses
    if (m > 0)
    {
        int n = 0;
        int bossPeakBin = peakBins[n];
        double nextPeak = peakBins[++n];
        int midBoundary = (nextPeak - (double)bossPeakBin) / 2.0 + bossPeakBin;

        if (nextPeak >= 0)
        {
            for (uint32_t i = 0; i < FFT_LEN; i++)
            {
                if (i <= bossPeakBin)
                {
                    binData[i].localPeakBin = bossPeakBin;
                }
                else if (i < midBoundary)
                {
                    binData[i].localPeakBin = bossPeakBin;
                }
                else // > boundary, calc next set
                {
                    bossPeakBin = nextPeak;
                    nextPeak = peakBins[++n];
                    if (nextPeak > bossPeakBin)
                        midBoundary = (nextPeak - (double)bossPeakBin) / 2.0 + bossPeakBin;
                    else // nextPeak == -1
                        midBoundary = FFT_LEN;

                    binData[i].localPeakBin = bossPeakBin;
                }
            }
        }
    }
}

void FFTEffect::scramble(fftw_complex* complexData, int dataSize, double width)
{
    int maxOffset = static_cast<int>(dataSize * width);

    juce::Random random;
    int randomInt = random.nextInt();

    std::vector<double> scrambledReal(dataSize);
    std::vector<double> scrambledImag(dataSize);

    for (int i = 0; i < dataSize; i++)
    {
        int offset = randomInt % (2 * maxOffset + 1) - maxOffset;
        int newPos = i + offset;

        if (newPos < 0) newPos = 0;
        if (newPos >= dataSize) newPos = dataSize - 1;

        scrambledReal[newPos] = complexData[i][0];
        scrambledImag[newPos] = complexData[i][1];
    }

    for (int i = 0; i < dataSize; i++)
    {
        complexData[i][0] = scrambledReal[i];
        complexData[i][1] = scrambledImag[i];
    }
}

void FFTEffect::smear(fftw_complex* complexData, int dataSize, double width)
{
    int smearWidth = static_cast<int>(width * dataSize);
    double totalOriginalEnergy = 0.0;

    // First Pass: Calculate total energy
    for (int i = 0; i < dataSize; i++)
    {
        std::complex<double> bin = { complexData[i][0], complexData[i][1] };
        totalOriginalEnergy += std::norm(bin);  // norm is magnitude squared
    }

    double sum = 0.0;
    int count = 0;
    int windowStart = 0;
    int windowEnd = -1;

    for (int i = 0; i < dataSize; i++)
    {
        while (windowEnd < i + smearWidth / 2 && windowEnd < dataSize - 1)
        {
            windowEnd++;
            std::complex<double> bin = { complexData[windowEnd][0], complexData[windowEnd][1] };
            sum += std::abs(bin);
            count++;
        }

        while (windowStart < i - smearWidth / 2)
        {
            std::complex<double> bin = { complexData[windowStart][0], complexData[windowStart][1] };
            sum -= std::abs(bin);
            count--;
            windowStart++;
        }

        double avgMagnitude = (count > 0) ? sum / count : 0;
        std::complex<double> bin = { complexData[i][0], complexData[i][1] };
        double originalMagnitude = std::abs(bin);
        double phase = std::arg(bin);

        // Limit the increase in magnitude to avoid excessive gain
        double newMagnitude = std::min(avgMagnitude, originalMagnitude * (1.0 + width));

        std::complex<double> smearedBin = std::polar(newMagnitude, phase);

        complexData[i][0] = smearedBin.real();
        complexData[i][1] = smearedBin.imag();
    }

    double totalSmearedEnergy = 0.0;
    for (int i = 0; i < dataSize; i++)
    {
        std::complex<double> smearedBin = { complexData[i][0], complexData[i][1] };
        totalSmearedEnergy += std::norm(smearedBin);
    }

    double energyRatio = (totalSmearedEnergy > 0) ? sqrt(totalOriginalEnergy / totalSmearedEnergy) : 1.0;
    for (int i = 0; i < dataSize; i++)
    {
        complexData[i][0] *= energyRatio;
        complexData[i][1] *= energyRatio;
    }
}

void FFTEffect::contrast(fftw_complex* complexData, int dataSize, float raiseVal) {
    double averageMagnitude = 0.0;
    double originalPower = 0.0;
    std::vector<double> magnitudes(dataSize);

    // First Pass: Calculate average magnitude and original power
    for (int i = 0; i < dataSize; i++) {
        double realPart = complexData[i][0];
        double imagPart = complexData[i][1];
        double magnitude = std::sqrt(realPart * realPart + imagPart * imagPart);
        magnitudes[i] = magnitude;
        averageMagnitude += magnitude;
        originalPower += realPart * realPart + imagPart * imagPart;
    }
    averageMagnitude /= dataSize;

    double raiseValPlusOne = 1 + raiseVal;

    // Second Pass: Apply contrast effect
    for (int i = 0; i < dataSize; i++) {
        double magnitude = magnitudes[i];
        double phase = std::atan2(complexData[i][1], complexData[i][0]);

        double magnitudeDifference = magnitude - averageMagnitude;
        double newMagnitude = averageMagnitude + copysign(pow(std::abs(magnitudeDifference), raiseValPlusOne), magnitudeDifference);

        complexData[i][0] = newMagnitude * std::cos(phase); // Real part
        complexData[i][1] = newMagnitude * std::sin(phase); // Imaginary part
    }

    double modifiedPower = 0.0;
    for (int i = 0; i < dataSize; i++) {
        modifiedPower += complexData[i][0] * complexData[i][0] + complexData[i][1] * complexData[i][1];
    }

    double normalizationFactor = (modifiedPower > 0) ? std::sqrt(originalPower / modifiedPower) : 1.0;

    // Third Pass: Normalize to conserve energy
    for (int i = 0; i < dataSize; i++) {
        complexData[i][0] *= normalizationFactor;
        complexData[i][1] *= normalizationFactor;
    }
}


void FFTEffect::setFxMode(FxMode inFxMode)
{
    m_fxMode = inFxMode;
}

void FFTEffect::setFxValue(float newValue)
{
    if (m_fxMode == FxMode::scramble)
    {
        m_scramblingWidth = newValue;
    }
    else if (m_fxMode == FxMode::smear)
    {
        m_smearingWidth = newValue;
    }
    else if (m_fxMode == FxMode::contrast)
    {
        m_contrastValue = newValue;
    }
}

void FFTEffect::setPitchShift(double semitones)
{
    double newAlpha = pow(2.0, semitones / 12.0);
    double newOutputBufferLength = round((1.0 / newAlpha) * (double)FFT_LEN);

    // --- check for change
    if (newOutputBufferLength == outputBufferLength)
        return;

    // --- new stuff
    alphaStretchRatio = newAlpha;
    ha = hs / alphaStretchRatio;

    // --- set output resample buffer
    outputBufferLength = newOutputBufferLength;

    // --- create Hann window
    windowBuff.resize(outputBufferLength);
    windowCorrection = 0.0;
    for (unsigned int i = 0; i < outputBufferLength; i++) {
        windowBuff[i] = 0.5 * (1.0 - cos((i * 2.0 * kPi) / (outputBufferLength - 1)));
        windowCorrection += windowBuff[i];
    }
    windowCorrection = 1.0 / windowCorrection;

    // --- create output buffer
    outputBuff.resize(outputBufferLength, 0.0);
}

double FFTEffect::process(double sample)
{
    bool fftReady = false;
    double output = 0.0;

    output = phaseVocoder.processAudioSample(sample, fftReady);

    if (fftReady)
    {
        fftw_complex* fftData = phaseVocoder.getFFTData();
        jassert(fftData != nullptr);

        if (enablePeakPhaseLocking)
        {
            // --- get the magnitudes for searching
            for (uint32_t i = 0; i < FFT_LEN; i++)
            {
                binData[i].reset();
                peakBins[i] = -1;

                // --- store mag and phase
                binData[i].magnitude = getMagnitude(fftData[i][0], fftData[i][1]);
                binData[i].phi = getPhase(fftData[i][0], fftData[i][1]);
            }

            findPeaksAndRegionsOfInfluence();

            // --- propagate phases 
            for (uint32_t i = 0; i < FFT_LEN; i++)
            {
                double mag_k = binData[i].magnitude;
                double phi_k = binData[i].phi;

                // --- horizontal phase propagation
                //
                // --- omega_k = bin frequency(k)
                double omega_k = kTwoPi * i / FFT_LEN;

                // --- phase deviation is actual - expected phase
                //     = phi_k -(phi(last frame) + wk*ha
                double phaseDev = phi_k - phi[i] - omega_k * ha;

                // --- unwrapped phase increment
                double deltaPhi = omega_k * ha + principalArg(phaseDev);

                // --- save for next frame
                phi[i] = phi_k;

                // --- if peak, assume it could have hopped from a different bin
                if (binData[i].isPeak)
                {
                    // --- calculate new phase based on stretch factor; save phase for next time
                    if (binData[i].previousPeakBin < 0)
                        psi[i] = principalArg(psi[i] + deltaPhi * alphaStretchRatio);
                    else
                        psi[i] = principalArg(psi[binDataPrevious[i].previousPeakBin] + deltaPhi * alphaStretchRatio);
                }

                // --- save peak PSI (new angle)
                binData[i].psi = psi[i];

                // --- for IFFT
                binData[i].updatedPhase = binData[i].psi;
            }

            // --- now set non-peaks
            for (uint32_t i = 0; i < FFT_LEN; i++)
            {
                if (!binData[i].isPeak)
                {
                    int myPeakBin = binData[i].localPeakBin;

                    double PSI_kp = binData[myPeakBin].psi;
                    double phi_kp = binData[myPeakBin].phi;

                    // --- save for next frame
                    phi[i] = binData[myPeakBin].phi;

                    // --- calculate new phase, locked to boss peak
                    psi[i] = principalArg(PSI_kp - phi_kp - binData[i].phi);
                    binData[i].updatedPhase = psi[i];// principalArg(PSI_kp - phi_kp - binData[i].phi);
                }
            }

            for (uint32_t i = 0; i < FFT_LEN; i++)
            {
                double mag_k = binData[i].magnitude;

                // --- convert back
                fftData[i][0] = mag_k * cos(binData[i].updatedPhase);
                fftData[i][1] = mag_k * sin(binData[i].updatedPhase);

                // --- save for next frame
                binDataPrevious[i] = binData[i];
                peakBinsPrevious[i] = peakBins[i];

            }
        }

        if (m_fxMode == FxMode::scramble)
        {
            scramble(fftData, FFT_LEN, m_scramblingWidth);
        }
        else if (m_fxMode == FxMode::smear)
        {
            smear(fftData, FFT_LEN, m_smearingWidth);
        }
        else if (m_fxMode == FxMode::contrast)
        {
            contrast(fftData, FFT_LEN, m_contrastValue);
        }

        phaseVocoder.doInverseFFT();
        fftw_complex* inv_fftData = phaseVocoder.getIFFTData();

        std::vector<double> ifft(FFT_LEN, 0.0);
        for (uint32_t i = 0; i < FFT_LEN; i++)
            ifft[i] = inv_fftData[i][0];
        
        if (outputBuff.size() == outputBufferLength)
        {
            resample(ifft.data(), outputBuff.data(), FFT_LEN, outputBufferLength, interpolation::kLagrange4, windowCorrection, windowBuff.data());
        }

        phaseVocoder.doOverlapAdd(&outputBuff[0], outputBufferLength);
    }
    return output;
}
