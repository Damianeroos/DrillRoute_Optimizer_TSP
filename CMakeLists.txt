cmake_minimum_required(VERSION 3.5)

project(PcbPathFinder VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets PrintSupport Xml)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets PrintSupport Xml)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        qcustomplot/qcustomplot.cpp
        qcustomplot/qcustomplot.h
        sa_options.ui
        sa_options.cpp
        sa_options.h
        tsp_problem.cpp
        tsp_problem.h
        brdtsploader.cpp
        brdtsploader.h
        helpers.cpp
        helpers.h
        algorithm/ialgorithm.h
        algorithm/alg_np.h
        algorithm/alg_np.cpp
        algorithm/alg_nn.h
        algorithm/alg_nn.cpp
        algorithm/alg_sa.h
        algorithm/alg_sa.cpp
        algorithm/alg_two_opt.h
        algorithm/alg_two_opt.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(PcbPathFinder
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET PcbPathFinder APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(PcbPathFinder SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(PcbPathFinder
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(PcbPathFinder PRIVATE
    ${CMAKE_SOURCE_DIR}/qcustomplot
)

target_link_libraries(PcbPathFinder PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::PrintSupport
    Qt${QT_VERSION_MAJOR}::Xml
)

set_target_properties(PcbPathFinder PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS PcbPathFinder
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(PcbPathFinder)
endif()
