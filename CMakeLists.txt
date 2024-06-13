cmake_minimum_required(VERSION 3.25)
project(2324L_19_s30518_FK)

set(CMAKE_CXX_STANDARD 20)
include(FetchContent)
set(BUILD_SHARED_LIBS FALSE)

FETCHCONTENT_DECLARE(
        SFML
        GIT_REPOSITORY  https://github.com/SFML/SFML
        GIT_TAG         2.6.1
)
FetchContent_MakeAvailable(SFML)

add_executable(2324L_19_s30518_FK main.cpp)

target_link_libraries(2324L_19_s30518_FK sfml-graphics sfml-audio sfml-system)

add_custom_command(
        TARGET 2324L_19_s30518_FK
        COMMENT "copy stuff"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:2324L_19_s30518_FK>
        VERBATIM
)
