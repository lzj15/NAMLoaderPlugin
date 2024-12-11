add_subdirectory(GUI)

target_sources(NAMLoaderPlugin
        PRIVATE
        PluginEditor.cpp
        PluginEditor.h
        PluginProcessor.cpp
        PluginProcessor.h
)

target_sources(NAMLoaderPlugin
        PRIVATE
        core/NAMProcessor.cpp
        core/NAMProcessor.h
)

target_sources(NAMLoaderPlugin
        PRIVATE
        tonestack/ToneStack.cpp
        tonestack/ToneStack.h
)

target_sources(NAMLoaderPlugin
        PRIVATE
        impulse-response/ImpulseResponse.cpp
        impulse-response/ImpulseResponse.h
)

target_sources(NAMLoaderPlugin
        PRIVATE
        noise-gate/NoiseGate.cpp
        noise-gate/NoiseGate.h
)

target_sources(NAMLoaderPlugin
        PRIVATE
        resampler/Resampler.h
)

target_sources(NAMLoaderPlugin
        PRIVATE
        state/StateManager.cpp
        state/StateManager.h
)

target_sources(NAMLoaderPlugin
        PRIVATE
        rms/meter.h
)
