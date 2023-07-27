
set(RPN_LANG_DIR ${CMAKE_CURRENT_LIST_DIR})
set(RPN_LANG_SRCS rpn-stack.cpp rpn-interp.cpp types-dict.cpp math-dict.cpp stack-dict.cpp logic-dict.cpp keypad-dict.cpp)

list(TRANSFORM RPN_LANG_SRCS PREPEND ${RPN_LANG_DIR}/src/)

#message(RPN_LANG_SRCS: ${RPN_LANG_SRCS})

