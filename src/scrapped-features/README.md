# Scrapped BE features

These are features that were intended to be added to BetterEdit, but for one reason or another were scrapped before Update 6.10. Some of them we're somewhat functional but I didn't have the time to polish them; some of them I never got to work in the first place. Regardless, I have left the code for the features here, so if anyone else feels like taking inspiration for making these features (or finishing the versions here), they can feel free to do so.

## Scripting

(source files in `scripting/`, example and testing scripts in `scripting/scripts/`)

This was planned for BetterEdit ever since 2021, but I never got it to work. The idea is that rather than making users wait for me to add some features like "create circle tool" and such in BE updates, they could instead just write simple scripts to do that for them. The point of scripting is to provide an user-friendly interface for interacting with the editor – very much like SPWN – except the scripts would run instantly in-game rather than being a separate compiled program like SPWN.

The current implementation here uses JavaScript as the scripting language. I experimented with creating my own language in the past, and many people suggested using Lua or Python for this, but personally I just like JS the most out of all existing well-known scripting languages. It's simple, easy to learn, and works fine for this. I used [QuickJS-NG](https://github.com/HJfod/quickjs-ng) as the engine, although you might have to use my fork if the main branch hasn't merged my PR fixing Windows Clang builds yet.

I don't remember exactly how far I got with this, but I do remember actually running scripts in-game. There might have been some weird edge-case bugs with some features, but mostly I think it was mostly about just finishing up the editor bindings and writing default scripts.

I really also wanted for the user to be able to customize the editor UI to add their own quick buttons for running scripts, rather than having to go through the "Run Script" popup every time. For example, there was going to be some way for them to add buttons to right side menu that would just run a script when clicked. (One way this could've been accomplished is by just reading a JSON file or something at startup)

I also planned on scripts being able to be run as a "background worker" by registering some callback functions for certain events. For example, you could write a script that runs code everything you select some objects.

## Better selection

(`BetterSelect.cpp`)

The idea here was to rework the GD warp controls to be more in line with modern image editors. Selection would by default stretch while being anchored to the opposite side (unlike GD, where the default is to the center), while also snapping to grid. However, I never got the grid snapping to work properly, as it for some reason never snapped correctly, and I could not figure out why.

## Trigger previews

Very simple idea: select a trigger, click a button, and it shows what that trigger does. Ideally with a pause button and little progression bar so you can check out every Move Trigger in excruciating detail.

## Wrap mouse around

The idea here was that if you were moving the editor camera around and your mouse went past the window edge, it would automatically wrap around to the other side, allowing you to scroll potentially infinitely without ever having to lift your mouse. Unfortunately the code just didn't work properly.
