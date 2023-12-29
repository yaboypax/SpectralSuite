SpectralSuite is a low-latency implimentation of a few of my personal favorite FFT-based spectral FX. With the dry wet at 1.0 and no effect depth, you're still in the FFT zone and your transients will start to smear. This is not a precise tool for audio restoration, but a creative effect for production and sound design.

This project was written in C++ using the JUCE framework and fftw3 for the DSP math. 
Resources for fftw3 are provided in the build system. You can obtain JUCE by:

- Cloning the JUCE repository: `git clone https://github.com/juce-framework/JUCE.git`
- Installing JUCE via a package manager (if available for your system).

Make sure to set the `JUCE_DIR` variable in the CMakeLists.txt to the path where JUCE is located.
