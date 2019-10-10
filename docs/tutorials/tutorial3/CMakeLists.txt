set(PROJECT_NAME tutorial3)

cmake_minimum_required(VERSION 3.2)
project(${PROJECT_NAME} VERSION 0.1.0 LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 11)
set (PROJECT_SRC
        ${PROJECT_NAME}.cpp
        ../utilities/tutorial_utilities.cpp
     )
add_executable(${PROJECT_NAME} ${PROJECT_SRC} )
target_include_directories(${PROJECT_NAME} PUBLIC BEFORE 
"/Users/kmoy001/libcellml-tutorials/library/include"
"/Users/kmoy001/libcellml-tutorials/utilities"
)
target_link_libraries(${PROJECT_NAME} /Users/kmoy001/libcellml-tutorials/library/lib/libcellml.0.2.0.dylib)
set_target_properties(${EXE} PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)