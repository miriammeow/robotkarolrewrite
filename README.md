This is a "simple" RobotKarol remake (uncomplete). It's made in C and should (if everything works as intended) be compatible with linux and windows (idk about mac so probably not but idk)
To compile this so it works on your system you can use (LINUX) "gcc -o robotkarolrewrite c_RobotKarolRewrite_Program.c" then run the program with ./robotkarolrewrite
On (WINDOWS) you have to use mingw64 or something that can compile the program. Edit: You can use Visual Studio with "cl" to compile it (tested)

It currently has support for player controls / movement, custom field size and ansi colors.
Placing Markers and

Critical errors:
- No character checking for keybind assignment
- A single character can be assigned to a multiple keybinds
- Not checked for memory leaks or any other faulty coding, need to do that still quickly

WARNING:
The exe and linux executables are usually out of date (good for you tbh, less errored code)
