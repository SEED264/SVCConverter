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
find_package(OpenCV REQUIRED)
find_package(nlohmann_json REQUIRED)

# Add targets
add_executable(${PROJECT_NAME} src/main.cc)
target_sources(${PROJECT_NAME} PRIVATE
    src/app.cc
    src/main_window.cc
    src/analog_value_panel.cc
    src/analog_value_dialog.cc
    src/binding_dialog.cc
    src/config_dialog.cc
    src/config_data.cc
    src/config_parser.cc
    src/parts.cc
    src/input_manager.cc
    src/input_recorder.cc
    src/setting_parser.cc
    src/util_function.cc
)

add_library(lodepng submodule/lodepng/lodepng.cpp)
target_include_directories(lodepng PUBLIC submodule/lodepng)

# Compile rc file
target_sources(${PROJECT_NAME} PRIVATE res/app_res.rc)

# Set include directories
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_include_directories(${PROJECT_NAME} PRIVATE submodule/rapidcsv/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${wxWidgets_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})

# Link required libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${wxWidgets_LIBRARIES}
    ${OpenCV_LIBS}
    nlohmann_json::nlohmann_json
    lodepng
    Hid
)

# Configure linker options
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /std:c++latest /utf-8 /w /MT /MP)
    target_link_options(${PROJECT_NAME} PRIVATE /SUBSYSTEM:WINDOWS)
else()
    target_link_options(${PROJECT_NAME} PRIVATE
        # -static
        -s
        -mwindows)
endif()