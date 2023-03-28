cmake_minimum_required(VERSION 3.23)
project(GraphBox)

set(CMAKE_CXX_STANDARD 17)

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "C:/Libraries/SFML-2.5.1/lib/cmake/SFML")
find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

add_executable(GraphBox GraphBox.cpp GraphBox.h Single.cpp Single.h GraphState.cpp GraphState.h graph/Node.h)
target_link_libraries(GraphBox sfml-graphics sfml-audio)
