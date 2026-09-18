cmake_minimum_required(VERSION 3.10.0)
project(PONG VERSION 0.1.0 LANGUAGES C CXX)

add_executable(PONG main.cpp)

target_include_directories(PONG PRIVATE
${CMAKE_SOURCE_DIR}/vendor/SDL3-3.4.16/include/
)

target_link_directories(PONG PRIVATE
${CMAKE_SOURCE_DIR}/vendor/SDL3-3.4.16/lib/x64/
)

target_link_libraries(PONG PRIVATE
SDL3
)

add_custom_command(TARGET PONG POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different
"${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL3-3.4.16/lib/x64/SDL3.dll"
"$<TARGET_FILE_DIR:PONG>"
)