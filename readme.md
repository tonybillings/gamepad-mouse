# GamepadMouse

## Overview

Just a quick and dirty solution for using a gamepad for controlling the cursor and performing some common functions.  Currently only supports Windows, however it wouldn't take much effort to port to *nix (specifically, how it gets/sets the cursor position and how it sends simulated keystrokes).  

## Configuration

Currently there are just two parameters you can change at the top of `main.cpp`, which of course means configuration requires recompilation.  A future enhancement would include reading config data from a file.  

## Building

Microsoft Visual Studio 2019 was used to write/build this code, but of course you could use another compiler to compile `main.cpp`...just be sure to include `third_party/glfw/include` and link against `third_party/glfw/glfw3_mt.lib` (for static linkage) or one of the other included GLFW libraries.  

## Running

The app was designed to run in the background, consuming very little CPU time when not being used, so you could create a task using Task Scheduler that executes GamepadMouse.exe after logon and enable the service when you'd like to use it by sending it a command (details below).  

Otherwise, just launch the executable whenever you want to use your gamepad to control the mouse, just remember that it starts up in a "disabled" state so you will still need to send it the command to enable itself.  

## Usage

When the app starts up, it does so in a "disabled" state, sleeping most of the time, waking up once a second to check the state of the keyboard to see if you are sending it a command.  There are four such commands:  

**CTRL+F9** - Terminate the GamepadMouse.exe process.  

**CTRL+F10** - Show the console window, which is helpful if you need to troubleshoot an issue.  

**CTRL+F11** - Disable the service, meaning your gamepad will no longer control the mouse (important if you're playing a game!)

**CTRL+F12** - Enable the service so that your gamepad will control the mouse and can be used to perform some common functions.  

To be as efficient as possible, the app wakes up just once a second to check the keyboard state, so that means you may have to hold the combination of keys for up to one second before the app recognizes the command being sent.  You will hear a distinct sound for both the enable/disable commands and you can always show the console for visual confirmation.  

## Enable/Disable via Gamepad

If the `ALLOW_ENABLE_VIA_GAMEPAD` parameter is set to true in `main.cpp`, you can toggle the enabled state of the service by pressing the X, Y, A, and B buttons simultaneously.  Like the keyboard-based commands, the app only checks the state of the gamepad for this command once per second when the service is currently disabled, so you may need to keep the buttons down for up to one second.  When the service is enabled, the gamepad poll rate is much faster (~200 Hz).  

## Gamepad Button Mapping

**Left Bumper** -> Right mouse button.  Can be mapped to the left mouse button by changing the `REVERSE_MOUSE_BUTTONS` parameter at the top of `main.cpp`.

**Right Bumper** -> Left mouse button.  Can be mapped to the right mouse button by changing the `REVERSE_MOUSE_BUTTONS` parameter at the top of `main.cpp`.

**Left Trigger** -> Decrease the volume.

**Right Trigger** -> Increase the volume.

**X Button** -> Minimize the window currently being hovered over by the mouse cursor.  

**Y Button** -> Maximize the window currently being hovered over by the mouse cursor.  

**A Button** -> Space key (which is often interpreted as an "OK" button in some applications).

**B Button** -> Enter/return key.

**D-Pad Up** -> Up arrow.  

**D-Pad Down** -> Down arrow.  

**D-Pad Left** -> Left arrow.  

**D-Pad Right** -> Right arrow.  

**Back Button** -> Select-all (CTRL+A).  

**Left Thumbstick Button** -> Copy (CTRL+C).  

**Right Thumbstick Button** -> Paste (CTRL+V).  

**Start Button** -> Windows key (which brings up the Start menu).  

**Back+Start Buttons** -> Show the virtual on-screen keyboard.  NOTE: requires this app to be run with administrative privileges!  

Currently there is no way to change the mapping via a config file, but using the existing code as a guide it wouldn't be hard to change the mappings to your liking!

## Cursor Movement

You can use either the left or right thumbstick to move the mouse and the angle of both sticks will be added together when calculating the final velocity.  That means if you move both sticks in the same direction it will make the mouse move faster in that direction (so don't move them in opposite directions or else the mouse may not move at all!)  If the cursor moves too fast/slow, you can adjust its speed by changing the `MOUSE_SPEED` parameter at the top of `main.cpp`.  

## Known Issue

If you reboot your computer, log into Windows either with no mouse plugged in or with absolutely no mouse movement at all, then the cursor will not render on the screen.  The cursor will exist and can still be controlled by the gamepad if this app is running and the service is enabled, but the cursor will remain invisible until a mouse is connected and it is moved at least once.  This is a known quirk with Windows that should be avoided with the call to ShowCursor() but unfortunately that doesn't work.  