cmake_minimum_required(VERSION 3.20)
project(MiniMap C)

set(PN ${PROJECT_NAME})

set(SDL2 "${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL2/lib/x64/SDL2.lib")
set(SDL2main "${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL2/lib/x64/SDL2main.lib")
set(GL "OpenGL32")
set(glew "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glew/lib/Release/x64/glew32.lib")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL2/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/vendor/glew/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/headers")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(${PN} src/main.c src/glutils.c src/osmloader.c)
target_link_libraries(
    ${PN}
        ${SDL2}
        ${SDL2main}
        ${GL} 
        ${glew})

#set(COMPILE_FLAGS ${COMPILE_FLAGS} "-g")
