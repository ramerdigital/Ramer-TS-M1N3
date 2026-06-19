cmake_minimum_required(VERSION 3.15)

# Get current year (YY) and month (MM)
string(TIMESTAMP CURRENT_YEAR "%y")
string(TIMESTAMP CURRENT_MONTH "%m")
string(TIMESTAMP CURRENT_YEAR_FULL "%Y")

# Strip leading zero from month for CMake project version compatibility
string(REGEX REPLACE "^0" "" CURRENT_MONTH_CLEAN "${CURRENT_MONTH}")

# Build number (manual override - increment as needed)
set(BUILD_NUMBER "0")

set(PROJECT_VERSION_STRING "1.0.0")
set(PROJECT_VERSION_CLEAN "1.0.0")

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum OS X deployment target")
    set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Target architectures on macOS")

    if(NOT CMAKE_OSX_SYSROOT)
        execute_process(COMMAND xcrun --show-sdk-path
                        OUTPUT_VARIABLE CMAKE_OSX_SYSROOT_DETECTED
                        OUTPUT_STRIP_TRAILING_WHITESPACE)
        set(CMAKE_OSX_SYSROOT "${CMAKE_OSX_SYSROOT_DETECTED}" CACHE PATH "macOS SDK path" FORCE)
    endif()

    # Enable Xcode scheme generation and default schemes to Release configuration
    set(CMAKE_XCODE_GENERATE_SCHEME ON)
    set(CMAKE_XCODE_SCHEME_LAUNCH_CONFIGURATION "Release")
endif()

project(ramer-ts-m1n3 VERSION ${PROJECT_VERSION_CLEAN})

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(modules)
include_directories(modules)

set(JUCE_FORMATS AU VST3)

juce_add_plugin(ramer-ts-m1n3
    COMPANY_NAME "Ramer Digital"
    BUNDLE_ID "com.ramerdigital.ramer-ts-m1n3"
    VERSION "${PROJECT_VERSION_STRING}"
    PLUGIN_MANUFACTURER_CODE RaDi
    PLUGIN_CODE Rdm3
    FORMATS ${JUCE_FORMATS}
    PRODUCT_NAME "Ramer-TS-M1N3"
    LV2URI https://github.com/ramerdigital/TS-M1N3
    ICON_BIG resources/logo.png
    MICROPHONE_PERMISSION_ENABLED TRUE
    MICROPHONE_PERMISSION_TEXT "This plugin requires audio input to process your instrument signal in real-time."
)

# create JUCE header
juce_generate_juce_header(ramer-ts-m1n3)

# add sources
add_subdirectory(Source)
include_directories(Source)
add_subdirectory(resources)

target_compile_definitions(ramer-ts-m1n3
    PUBLIC
    JUCE_REPORT_APP_USAGE=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(ramer-ts-m1n3 PUBLIC
    juce_plugin_modules
    BinaryData
)

# --- macOS / Xcode Code Signing Targets Configuration ---
if(APPLE AND CMAKE_GENERATOR MATCHES "Xcode")
    set(plugin_targets ramer-ts-m1n3_AU ramer-ts-m1n3_VST3)
    
    foreach(target IN LISTS plugin_targets)
        if(TARGET ${target})
            # Apply code signing identity if defined via CMake
            if(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY)
                set_target_properties(${target} PROPERTIES
                    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "${CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY}"
                )
            endif()
            
            # Apply Development Team ID if defined via CMake
            if(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM)
                set_target_properties(${target} PROPERTIES
                    XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM}"
                )
            endif()
            
            # Apply signing style (Manual / Automatic)
            if(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_STYLE)
                set_target_properties(${target} PROPERTIES
                    XCODE_ATTRIBUTE_CODE_SIGN_STYLE "${CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_STYLE}"
                )
            endif()
            
            # Ensure PROVISIONING_PROFILE_SPECIFIER is empty for manual/developer ID certificate signing
            set_target_properties(${target} PROPERTIES
                XCODE_ATTRIBUTE_PROVISIONING_PROFILE_SPECIFIER ""
            )
        endif()
    endforeach()
endif()
