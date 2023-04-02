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
