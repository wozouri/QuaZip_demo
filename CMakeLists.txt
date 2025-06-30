cmake_minimum_required(VERSION 3.16)

project(simple_QuaZip VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(QUAZIP_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/libs")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)






set(PROJECT_SOURCES
        main.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(simple_QuaZip
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET simple_QuaZip APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(simple_QuaZip SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(simple_QuaZip
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(simple_QuaZip PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)


# 方法1：尝试使用标准的 find_package（可能失败）
list(APPEND CMAKE_PREFIX_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/zlib"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/bzip2"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/quazip"
)



# 尝试查找标准的 CMake 包
find_package(ZLIB QUIET)
find_package(BZip2 QUIET)
find_package(QuaZip-Qt5 QUIET)
find_package(ZLIB)
if(ZLIB_FOUND)
    message(STATUS "zlib DIR: ${ZLIB_DIR}")
    target_link_libraries(simple_QuaZip PRIVATE ZLIB::ZLIB)
else()
    message(WARNING "zlib not found")
endif()


find_package(BZip2)
if(BZip2_FOUND)
    message(STATUS "BZip2 DIR: ${BZip2_DIR}")
    target_link_libraries(simple_QuaZip PRIVATE BZip2::BZip2)
else()
    message(WARNING "BZip2 not found")
endif()

find_package(QuaZip-Qt5)
if(QuaZip-Qt5_FOUND)
    message(STATUS "QuaZip DIR: ${QuaZip-Qt5_DIR}")
    target_link_libraries(simple_QuaZip PRIVATE QuaZip::QuaZip)
else()
    message(WARNING "QuaZip not found")
endif()

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.simple_QuaZip)
endif()
set_target_properties(simple_QuaZip PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS simple_QuaZip
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(simple_QuaZip)
endif()
