// @ts-check

/// @name Log on select
/// @by HJfod
/// @worker

// This is just an idea
editor.addEventListener("select", objs => {
    for (const obj of objs) {
        print(`Selected ${obj}`);
    }
});
