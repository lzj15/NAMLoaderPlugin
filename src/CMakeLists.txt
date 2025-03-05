target_sources(${PROJECT_NAME}
PRIVATE
    NeuralAmpModeler.cpp
    StatusedTrigger.cpp
    ToneStack.cpp
    NamEditor.cpp
    PluginEditor.cpp
    PluginProcessor.cpp
    EqEditor.cpp
    EqProcessor.cpp
    TopBarComponent.cpp
    architecture.hpp
)

add_subdirectory(CustomStandaloneWindow)
add_subdirectory(PresetManager)
