KeyPad SVG Structure
--------------------

The KeyPad can be freely constructed using any drawing methods.
Each pressable key element should be contained within a group - the group should have a class of "keypad_key" and an attribute "vfp:key_value", the value of which is set to the "value" of the key (string). The value of the key will be passed to the software upon a key press, and will be available from the getInput() method.