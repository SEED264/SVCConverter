cmake_minimum_required(VERSION 3.10.0)
# Set compiler to clang
set(CMAKE_C_COMPILER   clang)
set(CMAKE_CXX_COMPILER clang++)

project(SVCConverter CXX)

# Find required packages
find_package(wxWidgets REQUIRED)

# Add targets
add_executable(${PROJECT_NAME} src/main.cc)

# Set include directories
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_include_directories(${PROJECT_NAME} PRIVATE ${wxWidgets_INCLUDE_DIRS})

# Link required libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${wxWidgets_LIBRARIES})

# Configure linker options
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_options(${PROJECT_NAME} PRIVATE /SUBSYSTEM:WINDOWS)
else()
    target_link_options(${PROJECT_NAME} PRIVATE -static -s -mwindows)
endif()