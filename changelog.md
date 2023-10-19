# v5.0.0-alpha.10

 - Fix a crash that could happen when entering the editor
 - Correctly enable RGB Color Input

# v5.0.0-alpha.9

 - Fix preview mode screwing up moving objects on playtest pause
 - Port the scale UI from v4
 - Automatically hide startpos popup when deleting a startpos
 - Rewrote startpos switcher, should be more stable
 - Add startpos counter and moving between start positions
 - Play level button from startpos popup now works
 - Port LDM Object count to pause menu
 - Port RGB color input
 - Move some common dependencies to a new mod, EditorAPI
 - Fix View Tab dissapearing when returning to the editor
 - Do not set gravity for ball and spider in smart startpos
 - Make Extra Object Info dynamically update when changing object properties (Might cause lag)
 - Make Extra Object Info toggleable
 - Port next free offsets for colors and groups
 - Various stability fixes

# v5.0.0-alpha.8

 - Rewrote BetterEdit from the ground up (again)
 - Added Rotate Saws
 - Added New Color Select Menu
 - Added Playtest Lag Fix
 - Added Smart StartPos
 - Added Dash Orb Line
 - Added Editor Pulsing
 - Added Highlight Triggers
 - Added Portal Line Colors
 - Fixed View Tab
 - Fixed crash when exiting the editor
 - Fixed crash when playing a level with any object above 2.0
 - Added Zoom With Mouse to mac
 - Added Time instead of Percentage
 - Added StartPos Switcher
 - Added Extra Object Info

# v5.0.0-alpha.7

 - Add customizable keybinds support through Custom Keybinds dependency
 - Add hover tooltips to Swipe mode buttons if the MouseAPI mod is installed
 - Added some default keybinds like Rotate 45 degrees
 - Add info button for start pos tab

# v5.0.0-alpha.6

### Changes
 - Add Playtest Replay
 - Add logo
 - Now on MacOS

# v5.0.0-alpha.5

### Changes
 - Fix select filters not working with new Swipe mechanics
 - Fix some black objects being incorrectly colored with the new Swipe mechanics
 - Fix a crash when using Paste State on multiple objects
 - Fix a crash related to playing levels
 - Remove start pos button from the UI and instead move the functionality to its own tab under Edit
 - Make the UI properly be hidden when playtesting
 - Remove the scripting button due to the feature still being half-baked
 - Add info popup for the new Swipe selection mechanics
 - New colors for selection mode sprites
 - If you try to select objects but have filters enabled, now displays a warning
 - Tracking hooks now have lower priority, lowering the chance of conflicts with other editor mods

# v5.0.0-alpha.4

### Fixes
 - Fixed performance issue while moving around in the editor
 - Disabling percentage text now actually disables it
 - Fixed crash when exiting editor
 - Fixed script popup list not being aligned to the top
 - Other small performance improvements and fixes
 - Fixed multiple objects being removed getting reported as multiple events

# v5.0.0-alpha.3

### Additions
 - Add level percentage in editor
 - Full undo history that allows undoing anything
 - Better selection stuff
 - Add ability to enable blending from live color select
 - Scripting support
 - Extra object info
 - View tab (that doesn't contain anything yet)
 - Settings button in the editor pause menu

### Changes
 - Retroactively switched versioning to v5.0.0-alpha.X as Geode now supports that
 - Update to Geode v1.0.0-beta

# v5.0.0-alpha.2

### Additions
 - Add special start pos selector popup (will be cleaned up later)

### Changes
 - Update for Geode version to v0.4.4

# v5.0.0-alpha.1

### Additions
 - Start pos switcher in the editor

# v5.0.0-alpha.0

### Additions
 - Zoom with ctrl + mouse wheel, move horizontally with shift + mouse wheel
 - Current zoom level is displayed as text
