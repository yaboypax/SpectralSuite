cmake_minimum_required(VERSION 3.15)

project(SpectralSuite VERSION 0.0.1)

# Specify the JUCE location (JUCE_DIR "/path/to/JUCE")
set(JUCE_DIR "../JUCE")

# Add the JUCE module
add_subdirectory(${JUCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# If you have access to the VST2 sdk
# juce_set_vst2_sdk_path("/path/to/VST2_SDK")

juce_add_plugin(SpectralSuite
    COMPANY_NAME Ya Boy Pax
    PLUGIN_MANUFACTURER_CODE Paxx
    PLUGIN_CODE spcS
    # Plugin Formats: VST3, AU, etc.
    FORMATS VST3 AU Standalone #Add VST for VST2
    # Add your source files here
    PRODUCT_NAME "Spectral Suite"
)

juce_generate_juce_header(SpectralSuite)

find_package(FFTW3 REQUIRED)

# Define your project sources
target_sources(SpectralSuite
    PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/cplxd.h
    Source/FFTEffect.cpp
    Source/FFTEffect.h
    Source/fftw3.h
    Source/MathConstants.h
    Source/RandomIcon.h
    Source/sincostable.h
    Source/SSLookAndFeel.h
    Source/wpFFT.cpp
    Source/wpFFT.h
)

# Link with JUCE modules
target_link_libraries(SpectralSuite
    PRIVATE
    BinaryData
    juce::juce_analytics
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_box2d
    juce::juce_core
    juce::juce_cryptography
    juce::juce_data_structures
    juce::juce_dsp
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_opengl
    juce::juce_osc
    juce::juce_product_unlocking
    juce::juce_video
    # Add other JUCE modules or dependencies if needed
    FFTW3::fftw3
)

# add Binary Data
juce_add_binary_data(BinaryData SOURCES
    Resource/Timmana-Regular.tff
    Resource/dice.png
)


# Setup the VST3
juce_generate_juce_header(SpectralSuite)
target_compile_definitions(SpectralSuite PRIVATE
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0 # Set to 1 if you need a web browser
    JUCE_USE_CURL=0    # Set to 1 if you need CURL
)
