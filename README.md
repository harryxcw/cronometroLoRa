# cronometroLoRa
ski Timing System

This is a ski timing system, now has just the start and the finish but will be
Possible to add a intermediate gate

FIRST CHANGE THE SERIAL NUMBER "matricola" inside the sketches, both start and end. You need because if there are two of these system around you'll have no problems of cross talking

The start is based on a TTGO esp32 lora board, has a built in oled screen, then you have an input by just one button.
The time start when you open the start, inside there is a reed relay that chance state. You choose which you are with the only button

The finish gate is a TTGO promine lora (is an Arduino promini dev), with an accelerometer. When you touch the gate the end send a signal to the start and you'll have you're time. 

Is based on a synchronization process at the start, the start asks the end his millis() value and store it (knowing his own millis()), when the end gate is moved sends his mills to the start that does the maths and gives you the time.
This method ensures that the relative error is aproximately zero.

Every person at the start has is own "profile", he select who is with the button and just starts.

In future there will be an interface trough a telegram bot, the internet connection will be provided by a sim7000e module
