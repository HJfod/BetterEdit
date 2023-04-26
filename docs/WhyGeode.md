# Why is BetterEdit 5 Geode-only?

This is a question that has been asked a lot, and understandably so. If you've never heard of Geode before, and the first things you hear are that it doesn't work with Mega Hack v7, you'll probably be incredibly confused as to why BetterEdit is only available for such a mod loader. Alas, there is a reason for it - however, explaining it fully requires explaining practically the entire history of modern GD modding (I am not kidding). So before we delve too deep into why, here's a short and simple version:

## Short answer

You've likely encountered a bug caused by two mods before - something like menuAnim crashing because of Menu Shaders, GDShare's buttons being misplaced because of BetterEdit, or literally no other mod working together with Sai Mod Pack. **These are a fundamental issue in .DLL mods, and they are what Geode solves.**

On top of this, **Geode makes for a better user experience** in general as it makes installing mods much easier. Instead of the nightmare of trying to move the correct .DLL files in the correct locations (which vary between mod loaders), Geode mods are installed through the click of a button in-game. Geode also makes just using mods easier - you can configure their settings in one place, enable/disable them at runtime, and figure out what each mod does without needing to consult any Discord servers.

Unfortunately, this does come with the side effect that **Geode is incompatible with .DLL mods**. In practice, this means that .DLL mods need to be ported by hand over to Geode, which may take a while. Mega Hack v8 will be available on Geode, BetterEdit 5 will be available on Geode, but it will take a while. Rest assured, once the porting is done, we won't be looking back. **Geode is not an alternative to .DLL mods - it's a replacement.** It's what the next generation of mods will all be built on.

That's the short answer. If you're satisfied with that explanation, you can move on to installing Geode, or waiting until the mods you want are ported. For the ones who are sticking around to read the full story, strap in. This is not only going to be an explanation of why BetterEdit 5 is Geode-only, but why Geode exists in the first place.

## Background: How modding is even possible

Geometry Dash does not have official support for mods. Not even remotely - many of its internal components have not been built to be extensible, with the game full of hardcoded amounts everywhere. However, thanks to the wonders of computers, we can still make mods for the game anyways - provided we first built all of the tooling required to load them in the first place.

Perhaps surprisingly though, the bare minimum mod loader is actually incredibly simple to make, at least on Windows. It can actually be summarized in one single function call: `LoadLibraryA`. That is all that you need to load .DLL mods - which is pretty useful!

The downside is that `LoadLibraryA` *only* loads a .DLL mod, and that's it. It doesn't provide any utilities for working with all of the hardcoded properties inside GD, and neither does any other built-in part of the game or the operating system. As such, there are practically two options: include all of those utilities as part of the mod itself, or make the mod loader include support for them. Geode is the second solution; every .DLL mod is the first.

## Hook conflicts

In the very early days of modern modding, all mods bundled all of the utilities for working with the game within themselves. This means that each mod has its own copy of all the tools and only interacts with its own copy - which is fine, until they start doing something that requires sharing knowledge, like modifying a hardcoded value identifying where a button is on screen. If one mod changes the button, it has no way of letting other mods know it has done so - and as such, any other mod trying to modify the same button might be unable to do so, given it has been changed.

The most prominent way that mod conflicts arise don't really even have to do with GD in particular - they have to do with one library: MinHook. In essence, MinHook is the engine for all .DLL mods - it is what enables mods to do their modding. However, there exist two ways of what is called 'linking' MinHook - dynamically and statically. Dynamically linking MinHook is sharing the utility with other mods - all mods refer to the same MinHook instead of bundling its code within themselves. Unfortunately, many mods instead statically link MinHook, which results in what is known as *hook conflict*; or in practice, two mods just not working together at all.

The reason hook conflicts happen when statically linking MinHook is because even though the MinHook code is all bundled within the mod itself, that code is interacting with GD's code, which is shared across all mods (if it wasn't, each mod would be running in its own separate GD window). And sometimes, if two mods that statically link MinHook are trying to modify the same part of GD's code simultaniously, they will clash and end up overwriting each other. Best case scenario is that one or both of the mods ends up with limited functionality - worst case is a crash.

The solution to hook conflicts is linking MinHook dynamically, and requiring users to install `minhook.x32.dll` (the shared library). This fixes hook conflicts, as now there's only one library at once interacting with GD's code, which means that nothing else is overwriting it. Provided that only mods that dynamically link MinHook are in use, which is not remotely the case with most mods - many still link MinHook statically.

## Back when this didn't matter

There used to be a time when all of this hook conflict nonsense didn't really matter. In early 2021, modern modding was just getting its start, and there were only a few mods around, all with different purposes. As such, mods ended up working together after all, as any user only had a few loaded at most, and they were all distinct - one modified the editor, one modified the main menu, and they didn't conflict because they weren't doing anything similar.

However, the modding scene is no longer anything like it was in early 2021. There are now hundreds of mods, and many of them interact with the same parts of GD's code for slightly different reasons. As such, hook conflicts are now a real problem - and a troublesome one, as there isn't a good fix for it. Changing an older mod from statically linking MinHook to dynamically linking it requires making a new version of it - which you then need to distribute and convince people to use over the old, bug-prone version.

On top of this, hook conflicts are far from the only issue plaguing mods. Even if every mod dynamically linked MinHook, that still wouldn't solve many incompatabilities - GD's hardcoded nature is to thank for that.

GDShare's DLL version has a bug where if it's loaded after BetterEdit's DLL version, the export button for levels will be misplaced. This is because BetterEdit also adds new buttons in the level info screen, and causes GDShare to mess up its UI changes. BetterEdit has no way of letting GDShare know what changes it has made, and GDShare has no way of receiving such information. It just has to make guesses as to what state the screen might currently be in, and hope that it's correct - which it isn't.

The solution to this issue would of course be simple: add some way for BetterEdit to communicate with GDShare, and let it know that the layout of the screen has changed. However, there is no built-in mechanism for this - we need to implement it in some shared library, that both GDShare and BetterEdit can depend on. Similar issues exist in other mods - menuAnim crashes if loaded after Menu Shaders, Sai Mod Pack just doesn't work alongside anything, and many editor mods like Group ID Filter are incompatible with BetterEdit (which is why many have versions included in the mod itself).

All of these problems have the same root cause: mods have no way to communicate with each other. They have no shared systems to access hardcoded values in a manner that lets others know what has been done. They don't even have a way of checking what other mods are loaded.

The solution, of course, is to make mods be able to communicate with each other. This, however, is much easier said than done. In order for such to be feasible, there needs to be some shared library like `minhook.x32.dll` that provides all the utilities for communication. However, if we introduce a new library, then the user needs to install that for any mods using it to work, and it still doesn't guarantee a solution, as if the user loads even one mod that doesn't communicate what it does, that can cause the same problems.

## A smart little idea

GD modders have of course known the solution to all of these problems for years - actually well before modern modding even got started. All the way back in 2018 there was already a project started, now known as Hyperdash, that had some very interesting features: it was a mod loader that only loaded mods built for it specifically, and provided shared utilities for mods to communicate with each other. Exactly the solution to the problems mods are facing.

However, Hyperdash took a long time to make - so long that it still isn't out, and probably never will be. Getting into why would be a whole another essay in itself, but to cut it short: in 2021, as moddern modding was starting to take off with the release of BetterEdit v1, and the problems of mod incompatabilities were starting to hurt mod developers, a few developers (including myself) started working on a little project, then called Lilac and now known as Geode. Its purpose: a mod loader that provides shared utilities for mods to communicate with each other, to fix all of the issues mods were facing currently.

Geode is the shared library we modders have been looking for. It lets mods communicate with each other, say what changes they have made, and let other mods adapt to those changes, so that mods like menuAnim and Menu Shaders, which realistically should work together, can actually work together.

However, as with Hyperdash, Geode took a long time to make - initially started in Summer 2021, it's still in Beta, every day trying to inch closer to a full release. When that will happen, no one knows - probably around the same time as the full releases of Mega Hack v8 and BetterEdit v5.

The reason for why Geode has taken so long is multi-faceted, but in essence it's just this: while we're building shared utilities, we might aswell build a lot of them. Let's not just limit ourselves to fixing hook and UI conflicts, but also make installation a breeze, writing mods a piece of cake, and add cross-platform support. Geode has taken a long time, because its intention is to be the all-purpose modding toolkit - fix modding for both users and modders themselves.

## Geode-only

However, Geode does come with one unfortunate disadvantage: it's incompatible with all other mod loaders. This is by design, and the reasoning is explained by what was mentioned earlier - just one mod that statically links MinHook can break all other mods. Actually, with Geode it's even worse - Geode doesn't use MinHook and instead its own systems, so just one mod that uses anything else than Geode can break all Geode mods (and not only can it, it absolutely will).

The result of this is that all mods that you want to run on Geode have to be built for it, which means that old .DLL-only mods need to be ported - which might take a while. Many old mods are made of absolutely terrible code, and porting them properly requires basically rewriting them from scratch - not a big deal for small mods like GDShare, but a huge deal for large projects like BetterEdit.

The upside of all of this is that now, instead of worrying if mods work together, they pretty much always should out-of-the-box, and when they don't, fixing it is a breeze for developers and distributing the fixed update is nearly automatic. Geode might seem very stupid now, when not many mods have been ported yet, but given how much faster developing new mods is with it compared to .DLL mods, you can be assured that Geode will only get better, while eventually .DLL mods will become obsolete.

**The point of Geode is and has always been standardization** - to make mods work together, so players can use them without trouble. This will make for a rough transition period, but it should all be worth it in the end.
