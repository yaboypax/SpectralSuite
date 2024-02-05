SpectralSuite is a low-latency implimentation of a few of my personal favorite FFT-based spectral FX. With the dry wet at 1.0 and no effect depth, you're still in the FFT zone and your transients will start to smear. This is not a precise tool for audio restoration, but a creative effect for production and sound design.

This project was written in C++ using the JUCE framework and fftw3 for the DSP math. 

# Installing JUCE
## You can obtain JUCE by:

- Cloning the JUCE repository: `git clone https://github.com/juce-framework/JUCE.git`
- Installing JUCE via a package manager (if available for your system).

Make sure to set the `JUCE_DIR` variable in the CMakeLists.txt to the path where JUCE is located.

# Installing FFTW3

## Windows

Install vcpkg: If you haven't already installed vcpkg on your system, you can do so by cloning the vcpkg repository from GitHub and then running the bootstrap script. On Windows, this typically looks like:


git clone https://github.com/Microsoft/vcpkg.git

then run:

.\vcpkg\bootstrap-vcpkg.bat


Install FFTW3: Now, you can install FFTW3 using vcpkg. Run the following command:

./vcpkg install fftw3

This will download and install FFTW3 and its dependencies. If you need the FFTW3 library with single precision, you might use fftw3f, or for long double precision, fftw3l. For parallel FFTW (with MPI), you can use fftw3-mpi.

When building with cmake it might be necessary to build with the command:

cmake -DCMAKE_PREFIX_PATH="C:\path\to\vcpkg\packages\fftw3_x64-windows" ..

...or wherever vcpkg placed your FFTW3Config.cmake file.

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
