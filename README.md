# sdl2-test

a simple sdl2 project

its basically a playground for myself to get used to some things in C and have at least some visual sugar :)

**BE AWARE: I try to leave the code in a state where it builds and runs, but this doesnt mean it does what i intended to do!**

good place to start with sdl2: https://wiki.libsdl.org/

## some keys you might wanna checkout

 - w, arrow up -> move up
 - s, arrow down -> move down
 - d, arrow right -> move right
 - a, arrow left -> move left
 - b -> on holding down, makes background disapear (mainly for debugging)
 - r -> reload the stage script 
 
### Update 2021/4/19

- another iteration for the lua script use
  - moving the state into the config and make it persitent throughout the runtime of the application
  - implementing a simple reload function for a script (first iteration, simply hard code stuff)
  - now the script can be edited and the changes can be reloaded on runtime, without restarting the application
  - we can now make blocks visible/invisible on runtime 

### Update 2021/4/14

- implemting first iteration of loading stage setup from external lua file 
  - function needs some refactoring to make it more readable i guess but we will see
-removing code that was used to setup stage screens 

### Update 2021/4/7

- using the configuration struct to replace most of the #define values in sdl2-test.h
  - its not perfect but a start ...
  
### Update 2021/4/6

- adding functionality to read config values from an external file via LUA
  - this is the first iteration and not perfect but it give the general idea of getting settings from an external file so we could replace some hardcoded #define values 
### Update 2021/3/25

- restructure of the project layout, adding a src and include dir
- rework Makefile to handle the changes in the project structure
  - this is a Makefile for a standard FreeBSD make tool so it looks kinda strange
    compared to a GNU Makefile

### Update 2021/3/24

- adding an entrypoint for lua, since we might do some stuff with external script later on.

### Update 2021/3/23 

- moved internal stuff in a private header file 
- refactor functionality into seperate functions to get a api-like feeling 
- make structs opaque in public header
- TODO: fix some display bugs of the "player" sprite 

### Update 2021/3/22

 - moved helper functions into seperate file
 - added a header file
 - refinded collision tests a little but its not very sophisticated still
   - added "climbing" too
 
### Update 2021/3/21

 - moved event handling into seperate function (this might happen with all the main parts like event handling or redraw)
 - made some simple init functions for rects in the blocks , screens and stage
 - adding simply physics for jumping 
 
### Update 2021/3/20

 - added some structs to work with object and "screens", I need to refactor things if it should turn into something "playable"
 - added a color key so i can have opacity 
 - added simple collition detection for the added objects, the way that those objects are added needs to be made easier(maybe config file)


## Acknowledgement

Again, this is just here so i have a place to store it, there are no means of making that usefull to someone else!!!


Since this a simple test for myself i didnt bother to much with running through stuff when i picked the image for the background. So if someone has a problem with that i will take it off github. I simply wanted to have a layout to try stuff out. 

## background png: 
- url: https://spritedatabase.net
- copyright: mostllikely nintendo since its a megaman stage rip done by Ryan914 

## sprite png: 
- url: https://retro-sprite-creator.nihey.org/character 
- copyright: the hard work was done by nihey takizawa and the sprites where made by Famitsu and EnterBrain 

also nice to create a more usefull sprite is https://sanderfrenken.github.io/Universal-LPC-Spritesheet-Character-Generator/#



