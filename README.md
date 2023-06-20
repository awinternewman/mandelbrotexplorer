# This thing is slow and can't go extremely far. You probably shouldn't be using it
(It was mainly just for me to learn how to use SDL.. sorta)

Dependencies: [SDL2](https://github.com/libsdl-org/SDL/releases/)

Sorry! No musl support!

If you'd like to compile for windows, you should rename the main function to WinMain.

Linux build instructions:<br>

Make sure you have SDL2 installed.  
For Debian/ubuntu:  
`# apt install libsdl2-dev`  
Arch:  
`# pacman -S sdl2`  
Fedora:  
`# dnf install SDL2-devel`  


And now you can build it:  

`git clone https://github.com/awinternewman/mandelbrotexplorer.git`  
`cd mandelbrotexplorer && make`  
run with:  
`./mandelbrotexplorer`  

Default keybinds:<br>
z: zoom in  
o: zoom out  
q: increase quality  
w: decrease quality  
Arrow keys: move  
p: toggle colors  (default is normal, toggles pretty)
Escape: quit  
[: change colors in 'pretty' mode
]: change colors in 'pretty' mode
