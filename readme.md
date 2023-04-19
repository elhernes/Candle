Candle
-----------
GRBL controller application with G-Code visualizer written in Qt.

Changes from upstream denvi/Candle
* Added support for WHB04B pendant (https://a.co/d/ac5HEBN)
* Added macro processor inspired by cncjs macros
* merged in changes from a number of PR's in denvi/Candle
  * fedya/master (preference paths on mac/linux)
  * garuma/tcp-support (connect over TCP)
  * dorkable-forkable/yoiang-MacOS (macOS fixes)

Supported functions:
* Controlling GRBL-based cnc-machine via console commands, buttons on form, numpad.
* Monitoring cnc-machine state.
* Loading, editing, saving and sending of G-code files to cnc-machine.
* Visualizing G-code files.

System requirements for running "Candle":
-------------------
* Qt5
* macOS/Linux
* x86, arm - Apple Silicon, Intel, Raspberry PI, probably others
* Graphics card with OpenGL 2.0 support
* 120 MB free storage space

Build requirements:
------------------
Qt 5.4.2 (with MinGW/GCC compiler??)

Macro Support:
-------------

Macro support inspired by cncjs macro support and roughly follows that
syntax.  Probe commands and User Commands as saved through the
Settings panel are processed through the macro processor.  When the
macro processing is active, all g-code lines are first sent to the
processor for evaluation or variable expansion.   Lines that start
with a `%` character are evaluated as follows.

  1. First the lines are checked against keywords for special
 functions
  2. If a keyword is not matched, the line is sent to the math parser for variable
assignment and mathematical expressions

Lines that do not start with a `%` character are evaluated for
variable expansion.  Any sequence like `%{var}`  is expanded to the
value of `var` variable in the expression parser.

This line will add a variable `PLATE_THICKNESS` to the parser with a
value of 5.0;

```
%PLATE_THICKNESS = 5
```

These lines send g-code with variable expansion applied to
`PROBE_DISTANCE`, `PROBE_FEEDRATE_A`, and `PROBE_FEEDRATE_B`

```
; Probe rear, slight right
G38.2 X1 Y%{PROBE_DISTANCE} F%{PROBE_FEEDRATE_A}
G1 Y-1 F%{PROBE_FEEDRATE_B} ; back off a bit
```

The `xyz-probe.txt` file in the examples directory shows a cncjs macro
converted for use with Candle.

Downloads:
----------
Experimental versions:

* [Version 1.2b release](https://github.com/Denvi/Candle/releases/tag/v1.2b)


For GRBL v1.1 firmware

* Windows: [candle_1.1.7.zip](https://github.com/Denvi/Candle/releases/download/v1.1/Candle_1.1.7.zip)
* Linux: [candle_1.1.7.tar.gz](https://github.com/Denvi/Candle/releases/download/v1.1/Candle_1.1.7.tar.gz)

(for GRBL v0.9 and below use Candle 1.0)

Before creating new issue:
------
Candle works with CNC controlled by GRBL firmware, many problems can be solved by using proper version of GRBL, using proper configuration.

Please read GRBL wiki: 
- GRBL v0.9-: https://github.com/grbl/grbl/wiki
- GRBL v1.1: https://github.com/gnea/grbl/wiki

"Candle" main window:
![screenshot](/screenshots/screenshot_heightmap_original.png)
