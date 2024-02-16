cmake_minimum_required(VERSION 3.15)

project(SpectralSuite VERSION 0.0.1)

# Specify the JUCE location (JUCE_DIR "/path/to/JUCE")
set(JUCE_DIR "../JUCE")

# Add the JUCE module
add_subdirectory(${JUCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# If you have access to the VST2 sdk
# juce_set_vst2_sdk_path("/path/to/VST2_SDK")

juce_add_plugin(SpectralSuite
    COMPANY_NAME "Ya Boy Pax"
    PLUGIN_MANUFACTURER_CODE Paxx
    PLUGIN_CODE spcS
    # Plugin Formats: VST3, AU, etc.
    FORMATS VST3 AU Standalone #Add VST for VST2
    # Add your source files here
    PRODUCT_NAME "Spectral Suite"
)

juce_generate_juce_header(SpectralSuite)

# Check if the system is macOS and Homebrew was used to install fftw
# I could only get find_package to work correctly for Windows
if(APPLE)
    # Set the path and version to FFTW3 for macOS
    set(FFTW3_DIR "/usr/local/Cellar/fftw/3.3.10_1")
    set(CMAKE_PREFIX_PATH ${FFTW3_DIR};${CMAKE_PREFIX_PATH})
endif()

find_package(FFTW3 REQUIRED)

# Define your project sources
target_sources(SpectralSuite
    PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/Engine/FFTEffect.cpp
    Source/Engine/FFTEffect.h
    Source/DSP/fftw3.h
    Source/DSP/MathConstants.h
    Source/DSP/wpFFT.cpp
    Source/DSP/wpFFT.h
    Source/GUI/RandomIcon.h
    Source/GUI/SSLookAndFeel.h
    Source/GUI/SSLookAndFeel.cpp
)

# Link with JUCE modules
target_link_libraries(SpectralSuite
    PRIVATE
    BinaryData
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_dsp
    juce::juce_gui_basics

    # Add other JUCE modules or dependencies if needed
    FFTW3::fftw3
)

# add Binary Data
juce_add_binary_data(BinaryData SOURCES
    Resource/Timmana_Regular.ttf
    Resource/dice.png
)


# Setup the VST3
target_compile_definitions(SpectralSuite PRIVATE
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0 # Set to 1 if you need a web browser
    JUCE_USE_CURL=0    # Set to 1 if you need CURL
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_FORCE_USE_LEGACY_PARAM_IDS=1
)

