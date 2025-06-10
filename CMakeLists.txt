cmake_minimum_required (VERSION 3.10.0)

project (n-body_sim)
file(GLOB_RECURSE n-body_sim_CPPS src/*.cpp imgui/*.cpp nlohmann/json.hpp)

add_executable(n-body_sim ${n-body_sim_CPPS})

target_include_directories(n-body_sim PUBLIC src lib)
target_link_libraries(n-body_sim PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(n-body_sim PUBLIC Threads::Threads)

find_package(GLEW REQUIRED)
target_link_libraries(n-body_sim PRIVATE GLEW::GLEW)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
target_include_directories(n-body_sim PUBLIC ${OPENGL_INCLUDE_DIR})
target_link_libraries(n-body_sim PUBLIC ${OPENGL_LIBRARIES})

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(n-body_sim PRIVATE glfw)

