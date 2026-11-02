cmake_minimum_required(VERSION 3.24.0 FATAL_ERROR)

# VERSION is the single source of truth — the installer scripts read it too.
file(READ "${CMAKE_CURRENT_LIST_DIR}/VERSION" APP_VERSION)
string(STRIP "${APP_VERSION}" APP_VERSION)

project(KomodoDrop VERSION ${APP_VERSION} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS YES)
set(JUCE_ENABLE_MODULE_SOURCE_GROUPS ON CACHE BOOL "" FORCE)

# Xcode 27 dropped support for deployment targets below 12.0, so pick the oldest
# the installed toolchain will still accept. Mirrors what Gin does.
if(APPLE AND NOT CMAKE_OSX_DEPLOYMENT_TARGET)
    execute_process(COMMAND xcodebuild -version OUTPUT_VARIABLE xcodebuild_version_output ERROR_QUIET)
    if(xcodebuild_version_output MATCHES "Xcode ([0-9]+)" AND CMAKE_MATCH_1 GREATER_EQUAL 27)
        set(CMAKE_OSX_DEPLOYMENT_TARGET 12.0)
    else()
        set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13)
    endif()
endif()

add_subdirectory(modules/JUCE)
add_subdirectory(modules/Gin/modules)

#==============================================================================
# liboauthcpp: SmugMug's API is OAuth 1.0a. Gin used to vendor this library in
# gin_network, but dropped it, so KomodoDrop carries its own copy.
add_library(liboauthcpp STATIC
    3rdparty/liboauthcpp/src/base64.cpp
    3rdparty/liboauthcpp/src/HMAC_SHA1.cpp
    3rdparty/liboauthcpp/src/liboauthcpp.cpp
    3rdparty/liboauthcpp/src/SHA1.cpp
    3rdparty/liboauthcpp/src/urlencode.cpp)

target_include_directories(liboauthcpp
    PUBLIC  3rdparty/liboauthcpp/include
    PRIVATE 3rdparty/liboauthcpp/src)

# Upstream liboauthcpp still uses sprintf; don't let its warnings drown out ours.
if(NOT MSVC)
    target_compile_options(liboauthcpp PRIVATE -Wno-deprecated-declarations)
endif()

set_target_properties(liboauthcpp PROPERTIES FOLDER "3rdparty")

#==============================================================================
juce_add_gui_app(KomodoDrop
    PRODUCT_NAME        "KomodoDrop"
    VERSION             "${APP_VERSION}"
    COMPANY_NAME        "Roland Rabien"
    COMPANY_WEBSITE     "www.rabien.com"
    COMPANY_EMAIL       "roland@rabien.com"
    BUNDLE_ID           "com.rabien.KomodoDrop"
    ICON_BIG            "${CMAKE_CURRENT_SOURCE_DIR}/Target.png"
    ICON_SMALL          "${CMAKE_CURRENT_SOURCE_DIR}/Target.png"
    NEEDS_CURL          TRUE
    NEEDS_WEB_BROWSER   FALSE
    HARDENED_RUNTIME_ENABLED TRUE
    PLIST_TO_MERGE "<plist>
<dict>
  <key>NSAppTransportSecurity</key>
  <dict>
     <key>NSAllowsArbitraryLoads</key>
     <true/>
  </dict>
</dict>
</plist>")

juce_generate_juce_header(KomodoDrop)

target_sources(KomodoDrop PRIVATE
    Source/App.cpp
    Source/AsyncDialogs.cpp
    Source/Settings.cpp
    Source/SettingsComponent.cpp
    Source/Smugmug.cpp
    Source/UploadComponent.cpp
    Source/UploadWindow.cpp)

# Keeps the headers visible in IDE project trees alongside their .cpp files.
target_sources(KomodoDrop PRIVATE
    Source/AsyncDialogs.h
    Source/Settings.h
    Source/SettingsComponent.h
    Source/Smugmug.h
    Source/UploadComponent.h
    Source/UploadWindow.h)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source" PREFIX "Source" FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/App.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/AsyncDialogs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/AsyncDialogs.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Settings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Settings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/SettingsComponent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/SettingsComponent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Smugmug.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Smugmug.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/UploadComponent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/UploadComponent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/UploadWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/UploadWindow.h)

target_compile_definitions(KomodoDrop PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=1
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:KomodoDrop,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:KomodoDrop,JUCE_VERSION>")

# Release builds carry debug info so crash reports can be symbolicated. It lands
# in the dSYM / PDB / .debug file that Installer/build.sh archives, not in the
# binary that ships, so this doesn't bloat the installers.
if(MSVC)
    target_compile_options(KomodoDrop PRIVATE $<$<CONFIG:Release>:/Zi>)
    target_link_options(KomodoDrop PRIVATE $<$<CONFIG:Release>:/DEBUG /OPT:REF /OPT:ICF>)
else()
    target_compile_options(KomodoDrop PRIVATE $<$<CONFIG:Release>:-g>)
endif()

if(APPLE)
    set_target_properties(KomodoDrop PROPERTIES
        XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym"
        XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS "YES")
endif()

target_link_libraries(KomodoDrop PRIVATE
    liboauthcpp
    gin
    gin_metadata
    gin_network
    juce::juce_cryptography
    juce::juce_gui_extra
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags)
