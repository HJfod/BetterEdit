# BetterEdit

A mod that makes the GD Editor, well, Better.

# Features

 * Type in Editor L, Editor L2 and Z Order 
 * Type in current layer
 * Type in HSV
 * Advanced Scale
 * Auto-saving 
 * Layer locking, naming, visibility
 * Backups
 * Presets
 * Favourite objects
 * Remap IDs
 * View Percentage in the Editor
 * Visibility tab (preview pulsing objects, saw rotation, LDM etc.)
 * Redesigned Select Color menu
 * Double-Click to open Edit Object
 * Customize keybinds
 * Various bugfixes for GD bugs (scaling messing up object positions, editor freezing etc.)
 * And more!

# Download

Download is available on my Discord Server: https://discord.gg/K9Kuh3hzTC

# Building

1. Install [GDMake](https://github.com/HJfod/GDMake) (good luck)

2. `gdmake submodules add nodes https://github.com/HJfod/GD-Nodes -inc-src -no-header`
   
   `gdmake submodules add-src nodes *.cpp`

   `gdmake submodules add Zipper https://github.com/sebastiandev/Zipper`

   `gdmake submodules add-inc Zipper zipper`

   (you may also need to do `gdmake add-inc <path/to/zlib>` and `gdmake add-lib <path/to/zlib>` if you run into issues finding `zlib.h`)

   `gdmake submodules add-inc Cocos2d cocos2dx/support/zip_support`

3. `gdmake . -outlvl minimal -re` (the `-re` flag is not necessary after the first compile)
