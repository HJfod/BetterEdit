@Update Tag **BetterEdit v4.0.5**

As stated previously, this is just a bugfixdate for BE 4 to make it more usable while I work on BE 5. Although, since v4.0.4 was never officially released, some features new in regard to v4.0.3 are included. See the **Changelog** for details.

I don't know when BE 5 will be finished, but I hope this makes BE 4 a bit more bearable. Also included is a new **minified** version that removes a bunch of features that are unfinished / unstable / laggy, which should make it perform better for users on low-end devices. See the **Changelog** for information on what features the minified version removes.

**Changes for v4.0.4 (alpha):**

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

**Changes for v4.0.5:**

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
 - Removed the "Move Camera on Object Move" option due to no one liking it. It having 0 bitches, if you will.

```
---
```

**Changes for v4.0.5 (minimal):**

**Changes**
 - Removed portal line colors + portal line hiding due to the feature requiring a midhook (potential source of instability / lag)
 - Removed the color picker tool due to the feature requiring a `CCScheduler::update` hook (potential source of lag)
 - Removed keybind repeating due to the feature requiring a `CCScheduler::update` hook
 - Removed layer visibility and opacity options due to the features requiring a midhook
 - Removed context menu due to the feature being unfinished and requiring lots of potentially laggy stuff
 - Removed keybind indicators due to the feature possibly causing lag
 - Removed View LDM option due to the feature requiring a midhook
 - Removed Soft Saving due to the feature being unfinished and laggy
