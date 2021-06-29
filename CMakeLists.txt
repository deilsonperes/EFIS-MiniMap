cmake_minimum_required( VERSION 3.20 )
project( MiniMap )

set( PN ${PROJECT_NAME} )

set( SDL2 "vendor/SDL2/lib/x64/SDL2")
set( SDL2main "vendor/SDL2/lib/x64/SDL2main")
set( GL "OpenGL32")
set( glew "vendor/glew/lib/Release/x64/glew32")

include_directories( "vendor/SDL2/include" )
include_directories( "vendor/glew/include" )

add_executable( ${PN} src/main.c)
target_link_libraries(
    ${PN}
        ${SDL2}
        ${SDL2main}
        ${GL} 
        ${glew} )
