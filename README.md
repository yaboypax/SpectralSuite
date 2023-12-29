SpectralSuite is a low-latency implimentation of a few of my personal favorite FFT-based spectral FX. With the dry wet at 1.0 and no effect depth, you're still in the FFT zone and your transients will start to smear. This is not a precise tool for audio restoration, but a creative effect for production and sound design.

This project was written in C++ using the JUCE framework and fftw3 for the DSP math. 

# Installing Juce
## You can obtain JUCE by:

- Cloning the JUCE repository: `git clone https://github.com/juce-framework/JUCE.git`
- Installing JUCE via a package manager (if available for your system).

Make sure to set the `JUCE_DIR` variable in the CMakeLists.txt to the path where JUCE is located.

# Installing FFTW3

## Windows
Download FFTW3:

Visit the FFTW download page.
Download the latest version of FFTW3 for Windows (look for a file named like fftw-3.x.x-dllNN.zip, where NN is your architecture, e.g., 32 or 64).
Extract the Files:

Extract the downloaded .zip file to a known location on your computer.
Include in Your Project:

You can either include FFTW3 in your system's PATH or specify the path directly in your project's build configuration.
To include in the PATH, copy the .dll files from the extracted folder to a directory in your system's PATH.
## macOS
Using Homebrew:

If you don't have Homebrew installed, install it first from https://brew.sh/.
Open the Terminal and run: brew install fftw.
Manual Installation:

## Linux (Ubuntu/Debian)
Install via Package Manager:
Open your terminal.
Update your package lists: sudo apt-get update.
Install FFTW3: sudo apt-get install libfftw3-dev.
