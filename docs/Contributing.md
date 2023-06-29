# Contributing to BE

If you'd like to contribute to the development of BE, here's a short how-to:

## Adding new features

1. Add a toggle for the feature in [settings in `mod.json`](/mod.json#L9)
2. If the feature is small, add a source file for it in [`src/features`](/src/features), or if it's a large feature with multiple source files, make a folder for it
3. Add any resources in `resources` 
4. If the feature contains things that other features could benefit from, write those in some other directory under `src` instead and expose an API
