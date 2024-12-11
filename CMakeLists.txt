cmake_minimum_required(VERSION 3.28.1)
set(CMAKE_CXX_STANDARD 17)

project(NAMLoaderPlugin VERSION 0.0.1)

add_subdirectory(deps/JUCE)

include_directories(
        deps/NeuralAmpModelerCore/NAM
        deps/NeuralAmpModelerCore/Dependencies/eigen
        deps/NeuralAmpModelerCore/Dependencies/nlohmann
        deps/AudioDSPTools/dsp/ResamplingContainer
)

juce_add_plugin(NAMLoaderPlugin
        #VERSION
        #COMPANY_NAME
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT FALSE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE
        COPY_PLUGIN_AFTER_BUILD TRUE
        PLUGIN_MANUFACTURER_CODE Juce
        PLUGIN_CODE Dem0
        FORMATS VST3 Standalone
        PRODUCT_NAME "NAMLoaderPlugin"
)

juce_generate_juce_header(NAMLoaderPlugin)

target_sources(NAMLoaderPlugin
    PRIVATE
        deps/NeuralAmpModelerCore/NAM/activations.cpp
        deps/NeuralAmpModelerCore/NAM/convnet.cpp
        deps/NeuralAmpModelerCore/NAM/dsp.cpp
        deps/NeuralAmpModelerCore/NAM/get_dsp.cpp
        deps/NeuralAmpModelerCore/NAM/lstm.cpp
        deps/NeuralAmpModelerCore/NAM/util.cpp
        deps/NeuralAmpModelerCore/NAM/wavenet.cpp
)

add_subdirectory(src)

add_subdirectory(assets)

target_compile_definitions(NAMLoaderPlugin
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        NAM_SAMPLE_FLOAT
)

target_link_libraries(NAMLoaderPlugin
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        BinaryData
)


if (MSVC)
    target_compile_options(NAMLoaderPlugin PRIVATE
            "$<$<CONFIG:DEBUG>:/W2>"
            "$<$<CONFIG:RELEASE>:/O2>"
    )
else()
    target_compile_options(NAMLoaderPlugin PRIVATE
            -Wall
            "$<$<CONFIG:DEBUG>:-Og;-ggdb>"
            "$<$<CONFIG:RELWITHDEBINFO>:-Ofast;-march=x86-64-v3>"
            "$<$<CONFIG:RELEASE>:-Ofast;-march=x86-64-v3>"
    )
endif()
