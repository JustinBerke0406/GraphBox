cmake_minimum_required(VERSION 3.23)
project(GraphBox)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-static -static-libstdc++ -static-libgcc")

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "C:/Libraries/SFML-2.6.1/lib/cmake/SFML")
set(CMAKE_GENERATOR_PLATFORM Win32)
find_package(SFML 2.6.1 COMPONENTS graphics audio window system REQUIRED)

add_executable(GraphBox WIN32 GraphBox.cpp GraphBox.h Single.cpp Single.h graph/GraphState.cpp graph/GraphState.h graph/Node.h helper/InputHelper.cpp helper/InputHelper.h option_types/Slider.cpp option_types/Slider.h rendering/ViewRenderer.h rendering/ViewRenderer.cpp FileManager.cpp FileManager.h helper/ViewHelper.cpp helper/ViewHelper.h rendering/Renderer.cpp rendering/Renderer.h PhysicsEngine.cpp PhysicsEngine.h ModeHandler.cpp ModeHandler.h option_types/Checkbox.cpp option_types/Checkbox.h option_types/OptionType.h IClickable.h)
target_link_libraries(GraphBox sfml-graphics sfml-audio sfml-main)
