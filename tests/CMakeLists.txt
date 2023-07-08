cmake_minimum_required (VERSION 3.24)

include(${CMAKE_CURRENT_SOURCE_DIR}/../rpn-lang.cmake)

find_package(Catch2 3)

project (rpn-tests)

if (${Catch2_FOUND})
add_executable(rpn-controller ${RPN_LANG_DIR}/src/rpn-controller.cpp ${RPN_LANG_DIR}/src/cnc-app.cpp trpn.cpp)
set_target_properties(rpn-controller PROPERTIES
          CXX_STANDARD 17
          CXX_EXTENSIONS OFF
          )
target_include_directories(rpn-controller PRIVATE ${RPN_LANG_DIR})

add_executable(stack-test  ${RPN_LANG_SRCS} stack-test.cpp)
set_target_properties(stack-test PROPERTIES
          CXX_STANDARD 17
          CXX_EXTENSIONS OFF
          )
target_include_directories(stack-test PRIVATE ${RPN_LANG_DIR})
target_link_libraries(stack-test PRIVATE Catch2::Catch2WithMain)

add_executable(runtime-test ${RPN_LANG_SRCS} runtime-test.cpp )
set_target_properties(runtime-test PROPERTIES
          CXX_STANDARD 17
          CXX_EXTENSIONS OFF
          )
target_include_directories(runtime-test PRIVATE ${RPN_LANG_DIR})
target_link_libraries(runtime-test PRIVATE Catch2::Catch2WithMain)

endif()
