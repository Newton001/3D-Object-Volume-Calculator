cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(3D_Object_Volume_Calculator VERSION 0.1 LANGUAGES CXX)

# Enable automatic processing for UIC, MOC, and RCC
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick Qml Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick Qml Gui)

# Find VTK
find_package(VTK REQUIRED COMPONENTS
  CommonColor
  CommonCore
  FiltersSources
  InteractionStyle
  RenderingContextOpenGL2
  RenderingCore
  RenderingFreeType
  RenderingGL2PSOpenGL2
  RenderingOpenGL2
  IOImage
  IOLegacy
  InteractionStyle
  InteractionWidgets
  RenderingVolumeOpenGL2
  ImagingStatistics
)

if (NOT VTK_FOUND)
  message(FATAL_ERROR "Unable to find the VTK build folder.")
endif()


# Project sources
set(PROJECT_SOURCES
    src/main.cpp
    src/ui/qml.qrc
    src/app/fileloading.h
    src/app/fileloading.cpp
    src/app/vtkwindow.h
    src/app/vtkwindow.cpp
)

# Enable response file for Ninja builds
set(CMAKE_NINJA_FORCE_RESPONSE_FILE "ON" CACHE BOOL "Force Ninja to use response files.")

# Configure executable or shared library
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(3D_Object_Volume_Calculator
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(3D_Object_Volume_Calculator SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(3D_Object_Volume_Calculator
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Link Qt, VTK, and ITK libraries
target_link_libraries(3D_Object_Volume_Calculator
  PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::Quick
    ${VTK_LIBRARIES}
    ${ITK_LIBRARIES}
)

# Set application properties
set_target_properties(3D_Object_Volume_Calculator PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.3D_Object_Volume_Calculator
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Install rules
include(GNUInstallDirs)
install(TARGETS 3D_Object_Volume_Calculator
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Finalize for Qt 6
if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(3D_Object_Volume_Calculator)
    qt_finalize_executable(3D_Object_Volume_Calculator)
endif()

# vtk_module_autoinit is required to initialize VTK modules
vtk_module_autoinit(
  TARGETS 3D_Object_Volume_Calculator
  MODULES ${VTK_LIBRARIES}
)
