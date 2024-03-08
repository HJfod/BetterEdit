# Better Edit Lua Scripting

Lua scripting in the Geometry Dash Editor enables users to interact with the editor programmatically, emphasizing a user-friendly API. Please note that, as of now, this functionality is exclusively accessible on Windows.

## Usage

The scripts folder is located at `%GEODE%\geode\config\hjfod.betteredit\scripts`, up on entering the script select script popup a directory will be created on this location. As of right now the path is not customizable, however symlinking the `scripts` directory is supported (and recommended)

The "add file" button will copy a file to said path automatically

## Keybindings

In order to bind keys to a script, you first need to use the "update bindings" button, that will add new scripts to the bindings list and remove old ones.

# Writing Scripts

Scripts start with the definition of the `name` and `developer` fields, as follows:
```lua
--@name Hello World
--@developer User
```
They are regular lua comments.

## Intellisense
Using [Lua Language Server](https://luals.github.io/) ([VSCode](https://marketplace.visualstudio.com/items?itemName=sumneko.lua))

1. Download [.luarc.json](https://github.com/iAndyHD3/BetterEdit/blob/lua/.luarc.json) and place it in your `scripts` directory (or the root directory of your workspace).
1. Download [gd.lua](https://github.com/iAndyHD3/BetterEdit/blob/lua/scripts/gd.lua) and place it in your `scripts` directory.

From my testing on vscode, lua language sever only recognizes the `.luarc.json` file if it's in the root of the workspace, therefore you might need to open the `scripts` folder as a workspace on vscode (or as said, place `.luarc.json` at the root of your workspace)