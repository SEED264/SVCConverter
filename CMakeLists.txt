cmake_minimum_required(VERSION 3.10.0)
# Set compiler to clang
set(CMAKE_C_COMPILER   clang)
set(CMAKE_CXX_COMPILER clang++)

# Set resource compiler
if (WIN32 AND NOT MSVC)
    set(CMAKE_RC_COMPILER windres)
endif()

project(SVCConverter CXX)

# Find required packages
find_package(wxWidgets REQUIRED)

# Add targets
add_executable(${PROJECT_NAME} src/main.cc)
target_sources(${PROJECT_NAME} PRIVATE
    src/app.cc
    src/main_window.cc
    src/slider_value_dialog.cc
    src/parts.cc
)
# Compile rc file
if (WIN32 AND NOT MSVC)
    target_sources(${PROJECT_NAME} PRIVATE res/app_res.rc)
endif()

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