set( BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE )
set( SDL_LIBC ON CACHE BOOL "" FORCE )
set( SDL_TEST OFF CACHE BOOL "" FORCE )
add_subdirectory(SDL)
target_link_libraries(${PROJECT_NAME} SDL2::SDL2main SDL2::SDL2-static)

# GLEW
add_subdirectory(glew EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} libglew_static)

# ImGUI stuff
file(GLOB IMGUI_STANDARD_SRC_FILES ${CMAKE_SOURCE_DIR}/libs/imgui/*.cpp ${CMAKE_SOURCE_DIR}/libs/imgui/*.h)
message("Imgui Files: ${IMGUI_STANDARD_SRC_FILES}")
add_library(imgui STATIC
        ${IMGUI_STANDARD_SRC_FILES}
        ${CMAKE_SOURCE_DIR}/libs/imgui/backends/imgui_impl_sdl2.cpp
        ${CMAKE_SOURCE_DIR}/libs/imgui/backends/imgui_impl_sdl2.h
        ${CMAKE_SOURCE_DIR}/libs/imgui/backends/imgui_impl_opengl2.cpp
        ${CMAKE_SOURCE_DIR}/libs/imgui/backends/imgui_impl_opengl2.h
        )

target_include_directories(imgui PUBLIC
                            ${CMAKE_SOURCE_DIR}/libs/imgui
                            ${CMAKE_SOURCE_DIR}/libs/imgui/backends
                            ${CMAKE_SOURCE_DIR}/libs/SDL/include
        )
target_link_libraries(${PROJECT_NAME} imgui)
