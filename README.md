# sdl2-test

a simple sdl2 project

its basically a playground for myself to get used to some things in C and have at least some visual sugar :)

**BE AWARE: I try to leave the code in a state where it builds and runs, but this doesnt mean it does what i intended to do!**

good place to start with sdl2: https://wiki.libsdl.org/

help with most of the lua code: lua-users.org , and some videos on youtube of course

some videos about the struggle: https://youtube.com/playlist?list=PLoFYYypOGXqRtCW0Kq9wyxemMVEfD2sOo

---

## some keys you might wanna checkout

 - w, arrow up -> move up
 - s, arrow down -> move down
 - d, arrow right -> move right
 - a, arrow left -> move left
 - b -> on holding down, makes background disapear (mainly for debugging)
 - r -> reload the stage script (buggy still)
 - i -> toggle on/off of  debug infos on the screen (not very flexible at the moment)
 - t -> toggle on/off the rendering of solid tiles (also nice for debug and block edit via script)

## some kind of a roadmap ...
 
  - find a proper project structure, some things are just to stange but that's simply 
    me ... I guess
  - implementing basic features to debug (maybe some kind of console)
  - implementing more lua interaction features so we can make more changes by scripts
  - revisting the "game" design after we make the existing code more robust 

---

## Acknowledgement

Again, this is just here so i have a place to store it, there are no means of making that usefull to someone else!!!


Since this a simple test for myself i didnt bother to much with running through stuff when i picked the image for the background. So if someone has a problem with that i will take it off github. I simply wanted to have a layout to try stuff out. 

### background png: 
- url: https://spritedatabase.net
- copyright: mostllikely nintendo since its a megaman stage rip done by Ryan914 

### sprite png: 
- url: https://retro-sprite-creator.nihey.org/character 
- copyright: the hard work was done by nihey takizawa and the sprites where made by Famitsu and EnterBrain 

also nice to create a more usefull sprite is https://sanderfrenken.github.io/Universal-LPC-Spritesheet-Character-Generator/#

---

### Update 2021/05/15

- a lot of reworking on collision testing and drawing after some research online
  - splipting drawing and of sthing up in a few helper functions
  - spliting collition testing up in helper functions
  - some changes in structs to determine if player is on the ground or climbing
  - reworked drawing of blocks so we can now see if they are solid or block to be climbed in
- still some hickups with the collisions on solid to none solid blocks when it comes to moving into another screen

- first step to make the code more robust by adding stdint.h and use int32_t (thanks for pointing this out to [ronnie sahlberg](https://github.com/sahlberg), great teacher as always)

### Update 2021/05/06

 - working my way back to collision testing on solid blocks only
   - added integers for a block size and a used block count
   - changed fixed array of block back to a block pointer and allocate/reallocate size in the
     init function for screen and block. Also free the blocks in the __gc function for the lua
     metatable.
   - changed lua script to reflect the change by adding another "counter/index"

### Update 2021/05/04

 - fixed some nasty "bugs" regarding the Lua State
   - it was placed in the wrong structure so every time i wanted to close it broke
     because some of the things in LUA where already "gone" ...
 - restructured the project files again, this won't be te last time anyway
 - fixed the basic collision testing, this is still not good enough so I need to 
   rework some things regarding "world object" representation
 - removed some debug messages in collision testing

### Update 2021/04/29

- I fixed the reload problem for the stage by adding some more function to the hardcode part.
  also added a few c functions to simply change values on created rects
- Its now possible to load tile informations from a file that defines the tile for every screen.
- Added some flags to determine where we can "leave" a screen to enter the next or previous one.
- only tiles that are marked as solid get filled with a color when t key was pressed 
- collision testing is still buggy

### Update 2021/04/27

- it's not really an update, i still struggle with the representation of the stages/screens/blogs.
  But to make the lua integation less painful i need to rework it anyway, so i will go down the 
  "Tile" Road and will define a fixed amount of tiles.
  
- also started to test ways of moving functions that would be useful in lua (no c function), from 
  the script into the c code with a hardcoded string. So I might be able to restict the used libraries later on.

- moved away from the wrapper struct for now, I basically figured that its not needed, as long as  
  the lua state in open, a userdata won't be garbage collected. That said, i might change it all anyway.

- fixed a problem with the TTF Font that showed after a while caused by the bruteforce loading in 
  every update of the screen. The font lives now in the config and might be moved later to some place that deals with rendering text more useful.
  
### Update 2021/4/25

- still working on lua functionality
  - added some functions for metatables
  - added functions to call from lua and set data 
  - changed lua script to use functions 
- drawback, reload of the stages is broken now
  - since the lua state seems to not gc correctly the way the script is written
 
### Update 2021/4/23

- refactoring the way app is creating the configuration from a lua file
  - now there are functions to define a metatable and methods to asign values in lua 
  - now we only need to register those "libraries" once we create the lua state and can access them 
    in the scripts (for now only in the config script).  
### Update 2021/4/21

- adding more InApp debugging
  - a small info about collition detected (need more work to make it useable)

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





