cmake_minimum_required(VERSION 3.28.1)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(NAMLoaderPlugin VERSION 0.0.1)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_definitions(-DNAM_SAMPLE_FLOAT)
add_definitions(-DDSP_SAMPLE_FLOAT)

add_subdirectory(deps/JUCE)
add_subdirectory(deps/NeuralAmpModelerCore/Dependencies/eigen)

juce_add_plugin(${PROJECT_NAME}
    PRODUCT_NAME "Neural Amp Modeler"
    VERSION ${PLUGIN_VERSION}
    FORMATS VST3 Standalone
    COPY_PLUGIN_AFTER_BUILD TRUE
    )

juce_generate_juce_header(${PROJECT_NAME})

add_subdirectory(assets)
add_subdirectory(src)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        deps/NeuralAmpModelerCore/NAM
        deps/NeuralAmpModelerCore/Dependencies/nlohmann
        deps/AudioDSPTools/dsp
        deps/ff_meters
    )

target_sources(${PROJECT_NAME}
    PRIVATE
        deps/NeuralAmpModelerCore/NAM/activations.cpp
        deps/NeuralAmpModelerCore/NAM/convnet.cpp
        deps/NeuralAmpModelerCore/NAM/dsp.cpp
        deps/NeuralAmpModelerCore/NAM/get_dsp.cpp
        deps/NeuralAmpModelerCore/NAM/lstm.cpp
        deps/NeuralAmpModelerCore/NAM/util.cpp
        deps/NeuralAmpModelerCore/NAM/wavenet.cpp
        deps/AudioDSPTools/dsp/dsp.cpp
        deps/AudioDSPTools/dsp/ImpulseResponse.cpp
        deps/AudioDSPTools/dsp/NoiseGate.cpp
        deps/AudioDSPTools/dsp/RecursiveLinearFilter.cpp
        deps/AudioDSPTools/dsp/wav.cpp
        deps/ff_meters/ff_meters.cpp
    )

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_MODAL_LOOPS_PERMITTED=1
        JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP=1
        PLUG_VERSION="${PLUG_VERSION}"
    )

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        Eigen3::Eigen
        BinaryData
    )
