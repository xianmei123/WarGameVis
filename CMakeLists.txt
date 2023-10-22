cmake_minimum_required (VERSION 3.8)

SET_PROPERTY( GLOBAL PROPERTY USE_FOLDERS ON )

project ("the")

set(CMAKE_CXX_STANDARD 11)


set(GLFW_LIB ${CMAKE_SOURCE_DIR}/lib/glfw3.lib)


include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/*.cpp ${CMAKE_SOURCE_DIR}/*.c ${CMAKE_SOURCE_DIR}/jsonlib/*.c)

file(GLOB HEADERS ${CMAKE_SOURCE_DIR}/*.h ${CMAKE_SOURCE_DIR}/jsonlib/*.h)

#生成可执行程序
add_executable(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
    
)

target_link_libraries("the" ${GLFW_LIB})
