
cmake_minimum_required(VERSION 3.16)
project(nbody)

include(FetchContent)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(FETCHCONTENT_QUIET FALSE)

#
# glfw
#
set(GLFW_BUILD_DOCS FALSE)
set(GLFW_BUILD_EXAMPLES FALSE)
set(GLFW_BUILD_TESTS FALSE)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.4
  GIT_PROGRESS TRUE)

#
# glad
#
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad
  GIT_TAG v0.1.34
  GIT_PROGRESS TRUE)

#
# glm
#
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm
  GIT_TAG 0.9.9.8
  GIT_PROGRESS TRUE)

#
# fetch dependencies
#
FetchContent_MakeAvailable(glfw glad glm)

set(
        PROJECT_SOURCES
        main.cpp
	Particle.h
	Particle.cpp
	Window/Window.h
	Window/Window.cpp
	Window/WindowInputManager.h
	Window/WindowInputManager.cpp
        Shaders/ComputeShader.h
	Shaders/ComputeShader.cpp
	Shaders/Shader.h
	Shaders/Shader.cpp
	Shaders/VertexFragmentShader.h
	Shaders/VertexFragmentShader.cpp
	Camera/Camera.h
	Camera/Camera.cpp
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

add_dependencies(${PROJECT_NAME} glfw glad glm)

target_include_directories(
        ${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Window/
        ${CMAKE_CURRENT_SOURCE_DIR}/Shaders/
        ${CMAKE_CURRENT_SOURCE_DIR}/Camera/
)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad glm)