**Changes from v4.0.4 (alpha):**

**Additions**
 - Assign mouse buttons to keybinds
 - Redesigned move buttons menu
 - View dash orb lines in Visibility tab
 - Group summary popup that contains info about used groups and triggers; Note that this is currently only accessible through keybinds
 - Take screenshots inside the editor; Note that this is currently only accessible through the right-click menu and keybinds
 - LDM objects count in editor pause menu
 - Modify mixed Z Order, Editor Layer and Editor Layer 2 values without resetting them
 - Various new keybinds + keybind descriptions
 - Probably other additions that I never documented and have forgotten

**Changes**
 - Probably some changes that I never documented and have forgotten

**Experimental Features**
 - Right-click context menu with quick actions. Please note that this feature is extremely barebones, unfinished and buggy; it has been put on hold for BE 5.

**Bugfixes**
 - Fixed scale lock buttons not being clickable sometimes
 - Fixed saws not updating their rotation correctly when Preview Saws is on
 - Probably other bugfixes that I never documented and have forgotten

```
---
```

**Changes from v4.0.5 (alpha):**

**Additions**
 - Backup everything button that exports all of your levels as .gmd2 files

**Bugfixes**
 - Opening a level with a controller plugged in no longer crashes
 - Fixed crashing when holding keys with repeating enabled
 - Anything else that was fixed during the alpha releases
 - Fixed the infamous game not opening bug (see the first part of Changes for an important note)

**Changes**
 - Editor Layer settings (name, opacity, locking) are no longer saved due to it being likely the root cause of the bug where the game would not open. I apologize for the inconvenience
 - Removed favourites
 - Removed a bunch of unused/unfinished features & code, including but not at all limited to: Templates, Undo History, Paste Repeating, Integrated Console, etc.. Sorry if you liked these features, they'll come back in better(edit) quality for BE 5. Or not. Some of them were actually bad ideas.
 - Please note that regardless of if you do / don't have a Z order bypass enabled, the Z order on every object is limited to a value between -999 and 9999. The reason for this is because of a BE feature that allows mixed Z orders to be edited simultaniously, and that GD considers -1000 to be Mixed.
 - Saving order has been changed so that GD data is saved first and BE data after, so if BE data saving fails, your GD progress should still be kept.
