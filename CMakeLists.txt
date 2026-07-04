cmake_minimum_required(VERSION 3.21)
project(HardstyleVaultStutter VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option: use CPM or JUCE submodule. Adjust path as needed.
# This example assumes JUCE is a git submodule at ./JUCE
set(JUCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/JUCE")

if(NOT EXISTS "${JUCE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR "JUCE not found at ${JUCE_DIR}. Please init submodule: git submodule update --init --recursive")
endif()

add_subdirectory("${JUCE_DIR}" JUCE)

# Define the plugin
juce_add_plugin(HardstyleVaultStutter
    COMPANY_NAME "Dexkamp"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Dxkp
    PLUGIN_CODE HVST
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Hardstyle Vault Stutter"
)

# Add source files
target_sources(HardstyleVaultStutter PRIVATE
    src/PluginProcessor.cpp
    src/PluginEditor.cpp
    src/StutterEngine.cpp
    src/CustomLookAndFeel.cpp
)

# Link JUCE modules
target_link_libraries(HardstyleVaultStutter PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_dsp
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)

# Include directories
target_include_directories(HardstyleVaultStutter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Compiler flags for warnings
if(MSVC)
    target_compile_options(HardstyleVaultStutter PRIVATE /W4)
else()
    target_compile_options(HardstyleVaultStutter PRIVATE -Wall -Wextra -Wpedantic)
endif()
