cmake_minimum_required(VERSION 3.23)
project(GraphBox)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-static -static-libstdc++ -static-libgcc")

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "C:/Libraries/SFML-2.5.1/lib/cmake/SFML")
find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

add_executable(GraphBox GraphBox.cpp GraphBox.h Single.cpp Single.h graph/GraphState.cpp graph/GraphState.h graph/Node.h helper/InputHelper.cpp helper/InputHelper.h Slider.cpp Slider.h rendering/ViewRenderer.h rendering/ViewRenderer.cpp FileManager.cpp FileManager.h helper/ViewHelper.cpp helper/ViewHelper.h rendering/Renderer.cpp rendering/Renderer.h PhysicsEngine.cpp PhysicsEngine.h)
target_link_libraries(GraphBox sfml-graphics sfml-audio)
