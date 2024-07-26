# BetterEdit 6

## v6.7.0
 * Add button to quickly go to <cy>Next Free Layer</c>
 * There's now a small buffer period in the <cj>Save Level</c> keybind, so that spamming the save key will only save once
 * Bump version number to avoid having <cr>v6.6.6</c>

## v6.6.5
 * Fix <cj>Next Free</c> not taking into account trigger Target Group IDs on Android
 * Fix issues with BetterEdit not being loaded on Android due to <cb>Custom Keybinds</c> missing
 * Fix layer input misplacement when using <co>UI Scale</c>

## v6.6.4
 * Add keybinds for <cp>Increase Grid Size</c> and <cg>Decrease Grid Size</c>
 * Increase the hardcoded limits of <ca>Button Rows</c> and <cy>Buttons Per Row</c> to 12 and 48 respectively
 * Bump <cb>Custom Keybinds</c> version to add support for assigning mouse side buttons to keybinds

## v6.6.3
 * Add a <co>Support</c> button to the pause menu (that can be hidden through the popup)
 * Add <ca>Developer Mode</c> setting
 * Fix a bug with backups overwriting the level

## v6.6.2
 * Fix bugs with <cp>auto-save</c> that made it impossible to save any new progress as well as causing levels to be duplicated and made it impossible to delete levels

## v6.6.1
 * <cj>Auto-saved changes</c> are now <co>discarded if you exit without saving</c>
 * Fix <cj>Auto-Save countdown</c> being permanently stuck on screen
 * Fix <cy>Layer Controls</c> being clipped a little off-screen
 * Fix <cg>Group Offset</c> not working
 * Expand <cp>Changelog</c> to include historical versions of BetterEdit too for the sake of fun

## v6.6.0
 * Add keybinds for <cj>Save Level</c>, <co>Build Helper</c>, <cp>Align X</c>, <ca>Align Y</c>, <cb>Edit Object</c>, <co>Edit Group</c>, <cj>Edit Special</c>, <cr>Copy Values</c>, <cg>Paste State</c>, <cy>Paste Color</c>, and <cl>Toggle Link Controls</c>

## v6.5.0
 * Add support for <cy>2.206</c>
 * Add <cp>Quick Save</c> feature that significantly improves level saving speeds
 * Allow <cj>configuring autosave rate</c>
 * Fix autosave possibly causing the level to be corrupted if it occurred during playtesting
 * <cr>Remove BetterSave dependency</c> as the mod was discontinued

## v6.4.3
 * Fix playtest pausing not showing the UI

## v6.4.2
 * Fix accidentally using non-existant function
 
## v6.4.1
 * Add option for disabling the View Tab
 * Fix various visual glitches & bugs

## v6.4.0

 * Backups system
 * View tab
 * Better organized edit tab
 * Hide UI button
 * 1/4 and 1/8 moving is now relative to the current grid size

## v6.3.5

 * Add a border around the logo to have it fit with the Geode UI

## v6.3.4

 * <cr>Remove the "Crash Recovery" option</c> as it has not yet been implemented and was included accidentally
 * Fix <cp>Next Free Group ID Offset</c> crashing
 * Switch to the new BetterEdit logo

## v6.3.3

 * Make layer lock button fully hidden on All layer
 * Fix the new color selector causing crashes and being buggy
 * Fix Next Free Offset not being remembered
 * Fix grid size controls not being hidden on playtest

## v6.3.2

 * Disable grid size controls on 32-bit Android as the most likely source of a crash

## v6.3.1

 * Fix crash when placing objects, as well as grid size not working

## v6.3.0

 * Add Next Free Offset for Group IDs
 * Add Grid Size controls
 * Fix being able to click on the layer number to lock it
 * Fix crash when selecting colors using the new color selector

## v6.2.1

 * Fix crash on mobile when typing in layer number
 * Fix crash when re-entering editor

## v6.2.0

 * Bring back quarter & eighth block move keybinds as the root issue was fixed in Geode Beta 18
 * Add automatic update checking on startup
 * Add ability to copy Player Color channels in Copy Color (by [Alphalaneous](https://github.com/Alphalaneous))
 * Fix visual issues with layer locking
 * Other small bug fixes & improvements

## v6.1.1

 * Temporarily remove quarter & eighth block move keybinds as Geode is bugged and adding them caused moving objects to not work at all :/

## v6.1.0

 * Add in-game changelog popup
 * Add quarter & eighth block move keybinds
 * Fix crash when using the `Show Scale Control` keybind
 * Fix the layer input not being hidden during playtest
 * Fix being able to zoom during playtesting, which would make Platformer mode not work
 * Fix layer locking not being possible due to the layer input
 * Other small bug fixes & improvements

## v6.0.0

 * Add better color selection menu
 * Add better font selection menu
 * Add shift to scroll horizontally with the scroll wheel + control to zoom towards mouse
 * Add pinch to zoom on mobile
 * Add UI scaling
 * Add a lot of new keybinds (currently only on Windows)

# BetterEdit 5

> **Note: The changelog from here is for BetterEdit v5, which was only for Geode in Update 2.1 and is no longer available. The mod has since been rewritten from the ground up, and as such features mentioned in its changelogs may or may not exist in the latest version. v5 was also never officially released; only alpha versions were ever put out.**

## v5.0.0-alpha.8

 * Add Global Clipboard
 * Add Clear Clipboard
 * Add Use time instead of Percentage 
 * Fix trackpad horizontal scrolling on Mac

## v5.0.0-alpha.7

 * Add customizable keybinds support through Custom Keybinds dependency
 * Add hover tooltips to Swipe mode buttons if the MouseAPI mod is installed
 * Add some default keybinds like Rotate 45 degrees
 * Add info button for start pos tab

## v5.0.0-alpha.6

 * Add Playtest Replay
 * Add logo
 * Now on MacOS

## v5.0.0-alpha.5

 * Fix select filters not working with new Swipe mechanics
 * Fix some black objects being incorrectly colored with the new Swipe mechanics
 * Fix a crash when using Paste State on multiple objects
 * Fix a crash related to playing levels
 * Remove start pos button from the UI and instead move the functionality to its own tab under Edit
 * Make the UI properly be hidden when playtesting
 * Remove the scripting button due to the feature still being half-baked
 * Add info popup for the new Swipe selection mechanics
 * New colors for selection mode sprites
 * If you try to select objects but have filters enabled, now displays a warning
 * Tracking hooks now have lower priority, lowering the chance of conflicts with other editor mods

## v5.0.0-alpha.4

 * Fix performance issue while moving around in the editor
 * Fix crash when exiting editor
 * Fix script popup list not being aligned to the top
 * Fix multiple objects being removed getting reported as multiple events
 * Disabling percentage text now actually disables it
 * Other small performance improvements and fixes

## v5.0.0-alpha.3

 * Add level percentage in editor
 * Full undo history that allows undoing anything
 * Better selection stuff
 * Add ability to enable blending from live color select
 * Scripting support
 * Extra object info
 * View tab (that doesn't contain anything yet)
 * Settings button in the editor pause menu
 * Retroactively switched versioning to v5.0.0-alpha.X as Geode now supports that
 * Update to Geode v1.0.0-beta

## v5.0.0-alpha.2

 * Add special start pos selector popup (will be cleaned up later)
 * Update for Geode version to v0.4.4

## v5.0.0-alpha.1

 * Start pos switcher in the editor

## v5.0.0-alpha.0

 * Zoom with ctrl + mouse wheel, move horizontally with shift + mouse wheel
 * Current zoom level is displayed as text

# BetterEdit Legacy

> **Note: The changelog from here is for the original BetterEdit (DLL), which was only for Update 2.1 and is no longer available. The mod has since been rewritten from the ground up, and as such features mentioned in its changelogs may or may not exist in the latest version.**
> Old BetterEdit versions also did not follow a consistent versioning scheme; the versioning here reports what the actual version would've been according to the current scheme, and the (now incorrect) name the version was released under

## v4.3.0 Lite (v4.0.5-min)

 * Remove portal line colors + portal line hiding due to the feature requiring a midhook (potential source of instability / lag)
 * Remove the color picker tool due to the feature requiring a CCScheduler::update hook (potential source of lag)
 * Remove keybind repeating due to the feature requiring a CCScheduler::update hook
 * Remove layer visibility and opacity options due to the features requiring a midhook
 * Remove context menu due to the feature being unfinished and requiring lots of potentially laggy stuff
 * Remove keybind indicators due to the feature possibly causing lag
 * Remove View LDM option due to the feature requiring a midhook
 * Remove Soft Saving due to the feature being unfinished and laggy

## v4.3.0 (v4.0.5)

 * Backup everything button that exports all of your levels as .gmd2 files
 * Opening a level with a controller plugged in no longer crashes
 * Fix crashing when holding keys with repeating enabled
 * Fix the infamous game not opening bug
 * Anything else that was fixed during the alpha releases
 * Editor Layer settings (name, opacity, locking) are no longer saved due to it being likely the root cause of the bug where the game would not open. I apologize for the inconvenience
 * Remove favourites
 * Remove a bunch of unused/unfinished features & code, including but not at all limited to: Templates, Undo History, Paste Repeating, Integrated Console, etc.. Sorry if you liked these features, they'll come back in better(edit) quality for BE 5. Or not. Some of them were actually bad ideas.
 * Please note that regardless of if you do / don't have a Z order bypass enabled, the Z order on every object is limited to a value between -999 and 9999. The reason for this is because of a BE feature that allows mixed Z orders to be edited simultaniously, and that GD considers -1000 to be Mixed.
 * Saving order has been changed so that GD data is saved first and BE data after, so if BE data saving fails, your GD progress should still be kept.
 * Removed the "Move Camera on Object Move" option due to no one liking it. It having 0 bitches, if you will.

## v4.2.0 (v4.0.4)

 * Assign mouse buttons to keybinds
 * Redesigned move buttons menu
 * View dash orb lines in Visibility tab
 * Group summary popup that contains info about used groups and triggers; Note that this is currently only accessible through keybinds
 * Take screenshots inside the editor; Note that this is currently only accessible through the right-click menu and keybinds
 * LDM objects count in editor pause menu
 * Modify mixed Z Order, Editor Layer and Editor Layer 2 values without resetting them
 * Various new keybinds + keybind descriptions
 * Probably other additions that I never documented and have forgotten
 * Probably some changes that I never documented and have forgotten
 * Right-click context menu with quick actions. Please note that this feature is extremely barebones, unfinished and buggy; it has been put on hold for BE 5.
 * Fix scale lock buttons not being clickable sometimes
 * Fix saws not updating their rotation correctly when Preview Saws is on
 * Probably other bugfixes that I never documented and have forgotten

## v4.1.2 (v4.0.3)

 * Keymap now has the option to view QWERTZ layout
 * Fix the keybind layer crashing sometimes
 * Fix Create Guidelines From BPM crashing
 * Made the key texts in the Keymap better readable
 * Fix being able to input an empty keybind for modifiers
 * Objects no longer disappear with volume off when pulse is enabled
 * Fix keybind indicators showing up in the wrong places at the wrong times
 * Fix slider not working on objectless levels (Note: it still doesn't go past 100% - if you don't like that behaviour, disable the new slider from settings lol)
 * Fix hold to swipe not working
 * Fix RGB Color input crashing on invalid inputs
 * Checkpoint buttons now show their associated custom keybinds, if ones have been set
 * Remove an unnecessary midhook that was showing errors when BetterEdit was loaded

## v4.1.1 (v4.0.2)

 * You can now add a keybind to move object right half & move object up/down quarter

## v4.1.0 (v4.0.1)

 * Fully customizable keybinds!
 * Tons of new keybinds for things such as Build Helper, Preview Mode, Move by ½ a block etc.
 * Keybind indicators (hover a button to show its associated keybind, if it has one)
 * Keybind repeating (hold down a key to repeat it)
 * Double-Click to open up Edit Object, control to open up Edit Group & shift to open Edit Special
 * Move camera when an object is moved
 * You can now click on the little dots in the build tabs to go directly to the page
 * Removed Copy + Paste repeating due to instability & logic issues
 * Orbs not on B1 should now pulse
 * Saws now update their rotation when you modify them
 * Layer settings now save between sessions
 * Fix some settings not saving

## v4.0.0

 * Add Layer Tools (Locking, Naming, Opacity)
 * Add Copy + Paste Repeat
 * Add Hide / Show UI button
 * Add Bypass Object Limit Option
 * Editor Progress Bar now scales relative to the level
 * Create Guidelines from BPM
 * Pulse Objects & Rotate Saws in Editor
 * Add Remap IDs
 * Add Paste Objects From String
 * Add Relative Swipe
 * Add RGB Color Input (by Mat)
 * Add redesigned color select
 * Add HSV preview
 * Add color picker (pick any screen color)
 * Add auto-saving, progress recovery & level backups
 * Use the up arrow in the editor
 * Add Visibility Tab (toggle Preview Mode, LDM, etc.)
 * Add Automatic level backups
 * Highlight triggers when they are activated
 * Checked portals now have border colors that match the portal itself
 * Grid size buttons now also have a text input showing the current grid zoom level
 * Grid zoom limit removed
 * Removed Editor Position text
 * Zooming w/ Control + Scroll Wheel is a little less stupid
 * More Options to Customize BetterEdit
 * More options are saved between sessions
 * Textures moved to a spritesheet instead of individual files
 * If a texture is missing, the game will no longer crash and just use a placeholder instead
 * Now gives you more detailed information if the mod was unable to load for some reason
 * Clicking the "And You :)" name in BetterEdit settings now gives you a special achievement
 * Now using a more civilized version naming scheme
 * Also, now actually properly keeping track of what has been added, changed and fixed through doing proper Github commit messages
 * Now works on different aspect ratios
 * Fix Progress Bar not taking into account the end screen
 * Fix Create Color Triggers crashing on large levels
 * Various minor bugfixes

## v3.4.0 (v0.34)

 * You can set a start ID for the Next Free button in Groups & Colors
 * You can now toggle off UI elements like percentage, position, zoom text
 * Updated Paste State UI
 * Fix Paste State with scaling
 * You can now type negative numbers in Pickup Triggers (GD bug)
 * Various other minor bugfixes

## v3.3.0 (v0.33)

 * Add Editor Percentage
 * View the position you're at in the Editor
 * You can now disable the UI moving toward the cursor when scroll-zooming
 * Option to always enable custom grid size
 * Scale snap is now inputted as decimals instead of the fraction (you can now snap between 0.01 - 1)
 * Removed Herobrine
 * Fix advanced filter ranges option
 * Fix advanced filter scaling option
 * Other minor bugfixes

## v3.2.0 (v0.32)

 * Automatically create Color Triggers for each color channel (By HJfod & Mat)
 * Zoom in/out by holding Control and scrolling with the mouse (Courtesy of Mat ) & Shift + Scroll to scroll sideways
 * Add Extra Object Info
 * Add Custom Grid Size
 * Add a popup for customizing what properties to paste on Paste State
 * Scaling objects of different scales in the editor no longer fuck up the positions

## v3.1.0 (v0.31)

 * Add Global Clipboard
 * Allow reordering favorite objects
 * Settings are now actually saved

## v3.0.0 (v0.3)

 * Add Level presets
 * Add Advanced Filter
 * Add Favorite objects
 * Add Customizable snap
 * Add Editor volume controls
 * Add Icon Select in the scene before entering the editor
 * Fix a bug where the scale buttons weren't clickable after entering the editor through playtesting
 * Fix editor freeze glitch (by cos8o)

## v2.0.1 (v0.21)

 * Fix a bug where the layer input doesn't hide / show properly when playtesting

## v2.0.0 (v0.2)

 * Add Advanced Scaling
 * Fix bugs with Go To Layer

## v1.1.1 (v0.12)
 
 * Fix a bug where selecting multiple objects would set their Z Order to -1000

## v1.1.0 (v0.11)

 * Add HSV input
 * Add main screen Editor Layer input

## v1.0.0 (v0.1)

 * Add Editor Layer and Z Order inputs
