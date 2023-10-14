
# Contributing to BetterEdit

Thank you for your interest in contributing to this mod! Here are some guidelines you should follow when working with BE.

## Development team

BetterEdit is lead & managed by HJfod as the original developer of the mod. Alongside him there are other developers, who all have equal rights to contribute code, review pull requests, release updates, etc.. Development is mostly handled on [the BE 5 Discord server](https://discord.gg/UGHDfzQtpz).

If you'd like to join the dev team, the easiest way is to just express your interest in doing so [on the Discord](https://discord.gg/UGHDfzQtpz), or just open up a Pull Request. Joining the Discord is not at all mandatory to contribute code, but it is where all the devs can discuss together and get immediate feedback from users.

## Basic info about the codebase

BetterEdit 5 is developed using [Geode](https://geode-sdk.org), making it a required installation for all contributors. Aside from the basic Geode toolkit, BE currently doesn't require any additional special tools, though this may change in the future.

BetterEdit is split into two mods: **BetterEdit** and **EditorAPI**. The former is the mod you all know; it adds all the QoL features like Start Pos tools and better color selection menu. The latter is a general API mod that introduces a bunch of endpoints for adding stuff to the editor, such as new build tabs. This is where any features that other mods could depend on should go; BetterEdit should just be about using EditorAPI & other APIs to contribute its features.

An overview of the BE codebase's structure:

 - `api` is the folder for EditorAPI
 - `docs` is a bunch of documentation for developers, such as contribution guidelines
 - `resources` is where all resources such as sprites live
 - `src` is for all the source files related to BetterEdit
 - `src/features` is for all of BE's features
 - `src/other` is for other source files, such as utilities and such

The main unit of the BetterEdit codebase is a **feature**. This is stuff like Smart Start Pos, better color selection menu, View Tab, etc.. Each feature should be a **self-contained file or folder** inside the `src/features` directory. If the feature is small, such as additional keybinds, then it should just be one source file. If it needs multiple source files for legibility, it should be one folder - however, **all the code related to that feature should be contained within that folder**.

Features **should not normally depend on other features**. If it seems like a feature depends on another, consider the following:

 - Does the feature only depend on a small part of the other feature? Could that part be extracted into a shared API in either EditorAPI or `src/other`?
 - Is the feature really just an extension of the other feature? Should its code just be added to the original feature?

However, some features may be logically distinct and not extractable into a shared API, in which case depending on another feature is fine. The feature being depended on should be moved to a folder (if not already) and expose headers for the parts to depend on.

## Branches

The development of BetterEdit is handled using multiple development branches. In fact, **pushing directly to the `main` branch is disallowed**, and may only be done so through PRs that have succesfully passed [the build action](https://github.com/HJfod/BetterEdit/actions/workflows/build.yml). This is to ensure that the `main` branch always consists of code that has been tested and that succesfully builds - `main` should not be a development branch.

**Branches should mainly be made per-feature or per-contribution**, and only contain changes relevant for that one contribution. For example, this document was developed on an `updated-docs` branch, which only added new Markdown files under `docs`. **All development branches should be forks of `main`**, unless there is a good reason for a branch to be a fork of another development branch.

You should generally avoid making branches with a bunch of unrelated features like [HJfod's branch](https://github.com/HJfod/BetterEdit/tree/__SECRET_HJFOD_BRANCH_DO_NOT_TOUCH_OR_YOU_WILL_BE_FIRED), although doing so can be fine if you're just trying stuff out, or if you're HJfod and are working on super secret and super awesome stuff.

## What features should I add?

This is generally up to you! You can look for [features from `v4`](https://github.com/HJfod/BetterEdit/tree/v4) that haven't been ported yet, or [scroll the `#ideas` chat](https://discord.com/channels/1087452688956006471/1087452843725815938) on the Discord server to find any features that peek your interest.

However, there are some restrictions on what features you can add:

 - All features must be strictly editor-related. This is not a general GD improvement mod
 - Not all features from v4 were good ideas, and 1:1 parity is *not* a goal

Alongside these restrictions on what features you can add, there are also some guidelines on *how* you should implement them:

 - v4's implementations were generally bad - don't just blindly port them as-is, but think of how the features could be better presented, further expanded, more generalized, etc.
 - Prioritize ease-of-use and UX clarity - BetterEdit is meant to deobfuscate the editor
 - Avoid excessive hooking, especially with destructors, which may be unstable
 - Avoid byte patching and other platform-specific code at all costs
 - Add as much customization as possible, but always provide sane defaults
 - Don't reinvent the wheel

And as a final addition, here's an overview of **the philosophy of BetterEdit**:

 - BetterEdit is primarily a mod about **refinement** - *not* a major redesign of the entire editor
   - However, don't be afraid to disregard established muscle memory if the alternative is a significant improvement (such as the new edit tab with grouped arrows)
 - BetterEdit's target audience is primarily **intermediate to experienced creators** - while features that make the editor easier to learn for newcomers are welcome, they are not our main goal
 - BetterEdit aims to **follow GD's established UX and UI primitives** rather than break them
   - BetterEdit should still take pride in having good UX within GD's contraints
 - BetterEdit should not be constrained to any one area of the editor - anything that relates to building levels is within its limits
 - BetterEdit should make the editor better (as per its name)

## Adding a feature

Once you have a feature in mind that you believe fits all these standards, you can get by to implementing it! Here's the **basic implementation guide**:

 - Create a new branch with the name of your feature (for example `auto-color-triggers`)
 - Add a new source file for it under `src/features`, or a directory if it contains many files (for example `src/features/AutoColorTriggers.cpp` or `src/features/AutoColorTriggers/logic.cpp`)
 - Develope & test your feature
 - Add a new setting for the feature in [`mod.json`](https://github.com/HJfod/BetterEdit/blob/main/mod.json#L9) and make sure the feature is toggled by it
 - Make sure to note your changes in [`changelog.md`](https://github.com/HJfod/BetterEdit/blob/main/changelog.md)
 - [Open a Pull Request](https://github.com/HJfod/BetterEdit/pulls) once you are confident your feature is **ready for production**

Once you have opened your PR, other team members can test your branch and make sure it works as expected. Afterwards, if the build passes on all platforms, HJfod will merge it, or you may merge it yourself if you have the access to do so.

## Making a new release

Whenever you feel like it's time to make a new release, you should make a new branch bumping the version (if it has not already been bumped) and making sure the changelog is up-to-date, and then merge that branch to `main`. Afterwards, make a new release on Github and link it to the Discord server's [#releases channel](https://discord.com/channels/1087452688956006471/1087453005101678823). The release's description on Github should include a copy of the changelog for that release, as well as thank all new contributors on that release.

When bumping the version, be careful to follow [semver](https://semver.org/) (particularly with EditorAPI - BetterEdit's versioning may be much more laxed, especially when it comes to major versions). If you're in doubt about how the version should be updated, ask HJfod.

## Code style guidelines

In general, [follow Geode's code style guidelines](https://docs.geode-sdk.org/source/styling/). In addition, BetterEdit has some specific extra notes:

### Always use braces for `if`, `for`, and `while` statements, even if the body is just one piece of code
```cpp
// Correct
if (...) {
    stmt
}

// Wrong
if (...)
    stmt
```
The only exception is if the body is a return statement:
```cpp
// Correct
if (...)
    return ...;

// This is correct too though; the above case is usually only for the initial check for Super::init failing
if (...) {
    return ...;
}
```

### Field names should not use the `m_` prefix, since it is already included in `m_fields`:
```cpp
// Correct
int thing;
...
m_fields->thing;

// Wrong
int m_thing;
...
m_fields->m_thing;
```

### Member accesses should never use `this->`, member function calls should always have `this->`
```cpp
// Correct
m_stuff;
m_fields->thing;
this->updateUI();

// Wrong
this->m_stuff;
this->m_fields->thing;
updateUI();
```

### Switch cases longer than one statement should be wrapped in braces
```cpp
switch (thing) {
    case x: {
        ...
    } break;
}
```

### Avoid too many line breaks, but separate logically distinct things
```cpp
// Correct
if (...) {
    ...
}
callThing();

this->updateUI();
this->updateSaws();

// Wrong
if (...) {
    ...
}

callThing();
this->updateUI();

this->updateSaws();
```

### Every file should have one empty line at the end
That's it

### Long macro definitions should use multiple lines with the backticks aligned and follow same rules as other stuff
```cpp
// Correct
#define MACRO(x) \
    if (x) {     \
        ...      \
    }

// Wrong
#define MACRO(x) if (x) {...}
```
