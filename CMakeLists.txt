cmake_minimum_required (VERSION 3.8)

project (Main CXX)

set(PROJECT_NAME Main)

set(no_group_source_files
    "Shaders/Shader.frag"
    "Shaders/Shader.vert"
)

set(src
    "Src/Control.cpp"
    "Src/Globals.cpp"
    "Src/Main.cpp"
    "Src/Objects.cpp"
    "Src/Shader.cpp"
)

set(incl
    "Include/Control.h"
    "Include/Globals.h"
    "Include/Objects.h"
    "Include/Shader.h"
)

set(ALL_FILES
    ${src}
    ${incl}
)

set(glm_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Libs/glm-0.9.9.8/cmake/glm")

find_package(OpenGL REQUIRED)
find_package(glm CONFIG REQUIRED PATHS "${glm_DIR}")


add_executable (${PROJECT_NAME}  ${ALL_FILES})


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/Libs/freeglut/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/Libs/glew-2.1.0/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/Libs/glfw-3.3.8.bin.WIN64/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/Libs/glm-0.9.9.8/glm"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/Libs/freeglut/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/Libs/glew-2.1.0/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/Libs/glfw-3.3.8.bin.WIN64/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/Libs/glm-0.9.9.8/glm"
    )
endif()


if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/Libs/glfw-3.3.8.bin.WIN64/lib-vc2022;"
        "${CMAKE_CURRENT_SOURCE_DIR}/Libs/glew-2.1.0/lib/Release/x64;"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/Libs/glfw-3.3.8.bin.WIN64/lib-vc2022;"
        "${CMAKE_CURRENT_SOURCE_DIR}/Libs/glew-2.1.0/lib/Release/x64;"
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
#target_link_libraries(${PROJECT_NAME} PRIVATE "${GLUT_glut_LIBRARY}")
target_link_libraries(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Libs/freeglut/lib/x64/freeglut.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Libs/freeglut/lib/x64/freeglutd.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Libs/glew-2.1.0/lib/Release/x64/glew32.lib")


