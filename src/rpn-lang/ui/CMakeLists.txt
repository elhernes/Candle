cmake_minimum_required(VERSION 3.24)
project(rpn-test-ui)
include(${CMAKE_CURRENT_SOURCE_DIR}/../rpn-lang.cmake)

# set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall ")

if(APPLE)
    add_compile_definitions(Q_OS_MAC)
endif()
add_compile_definitions(APP_VERSION="1.1.9")
add_compile_definitions(QT_NO_DEBUG)

find_package(Qt5 COMPONENTS Core Widgets)

if (${Qt5_FOUND})
include_directories(${RPN_LANG_DIR})

#if(APPLE)
#    # If we switch to automatic PList generation
#    set(MACOSX_BUNDLE_ICON_FILE candle.icns)
#    
# 
#    set(MACOSX_BUNDLE_ICON_LOCATION ${PROJECT_SOURCE_DIR}/candle.icns)
#    set_source_files_properties(${MACOSX_BUNDLE_ICON_LOCATION} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
# 
#    add_executable(Candle MACOSX_BUNDLE ${SRC_FILES} ${SHADER_RSC} ${MACOSX_BUNDLE_ICON_LOCATION})
#    set_target_properties(Candle PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/Info.plist)
#else()
#endif()

add_executable(rpn-test-ui ${RPN_LANG_SRCS}
  rpnkeypad.cpp main.cpp
  rpnkeypad.ui
  rpnkeypad.h
  rpn-ui.qrc
  )
set_target_properties(rpn-test-ui PROPERTIES
          CXX_STANDARD 17
          CXX_EXTENSIONS OFF
          )

target_link_libraries(rpn-test-ui
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
    )

endif()