
RPN Controller - a generic high level RPN stack machine and languge.

Design Goals

1. Handle parsing, compilation, and execution of RPN/RPL stack based language

2. Only high level stack and application words (direct memory words
   not implemented)

3. Direct dispatch of native c++ word definition.  
 
4. Extensible with native (c++) words
Relatively simple - just add words to the dictionary.  Semantics are
stil in flux.

5. Extensible with user data types at (c++) compile time
Trickier, especially when combined with (#3) above.

Potential designs:

1. Pure Object - The 'stack' is a container of abstract StackObject items.  Can be extended by
just subclassing 'StackObject'.

Advantages:
- simple for the application designer to implement.
- well established paradigm (java, objective-c, Qt, other frameworks)
- infinitely extensible - add as many subclasses as you need

Disadvantage:
- not clear how the direct dispatch mechanism can properly downcast 

2. Pure Variant - The 'stack' is a container of 'std::variant' items.
   Can be extended by adding to the variant.
   
Advantages:
- simple to deal with at the parser controller level

Disadvantages:
- Extending by the applicaiton will require changing the core
  'std::variant' type.
  
3. Hybrid - stack uses the variant for 'primitive' types, one of which
   is 'StackObject' that lets applications subclass and extend.

Disadvantages-
* may not really help the dispatch problem, just pushes it off to the
  application
  
4. Codegen - define user words in a way that a codegen can create the
   intermediates.
   
-------------------------
  rpn::Stack::Object and friends

-----------------------------------------------------------
  13 Jun - To do

* add unit tests for type words
* implement the rest of the type words
* implement DO/WHILE DO/UNTIL loops
* implement FOR/STEP loops 
* finish / test progn compiles
  * nested loops in a word definition
  * nested FOR/DO loops
* implement IF/THEN/ELSE/ENDIF construct
* implement shunting-yard algebraic parser
* implement local variables (??)
* implement STO/RCL memory words
* add StackValidator comparison for early detection of conflicting definitions
* convert UI glossary button(s) to menu items (STACK/MATH/LOGIC/TYPES)
* add UI word to add menus and menu items
* add word to undefine a word
