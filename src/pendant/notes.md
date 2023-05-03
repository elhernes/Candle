## Notes on WHB04B

### Key Chords
The protocol supports two-key combinations, this generally works for the FN key.  However, it can work with any two key combinations.  The catch is that the detection/processing algo needs to be modified to handle it.

Currently, the algo passes every UI event to the app, and the app has a single switch statment for every two-key comination. For convenience to the implementation, it treats 'key_none' as a an event.  For single keypresses, the event sequence goes like:

1. Key press
2. Key release, but the event does not indicate what key was previously pressed.

For dual keys, the sequence is

1. Key 1 press
2. Key 2 press
3. Key release (could be one or two, I believe the key still pressed shows in the event)
4. Key release (shows no keys).

for a function+key, this is what happens:

1. FN key pressed (handler ignores)
2. Second key pressed (handler detects FN+Key), is processed
3. Key released (ignored)
4. Key released (ignored)

Based on this, some fairly sophisticated chording and ordering can be possible.

## Key + rotary

The keycodes are also contained in the rotary events.  From this it is also possible to use key + rotarty events.  The ones that seem to be the most obvious are Feed and Speed.  Holding one of the Feed+, Feed-, Speed+, Speed- buttons and turning the knob seem pretty natural way to adjust the overrides or maybe the speed/feed directly.

## Steps on the right hand knob.

Should the UI settings for step/continuous and step size be controlled by this knob?  Since the knob is a physical button and the UI subject to change.  It seems like they should stay disconnected.  I don't think I would reasonably expect to adjust the UI drop down by turning the rotary.  I'm either in the mental state of "jog with the UI" or "jog with the pendant" and they should stay separate.
