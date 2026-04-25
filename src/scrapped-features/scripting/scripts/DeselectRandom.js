// @ts-check

/// @name Deselect random
/// @by HJfod

const objs = editor.getSelectedObjects();
print(`Selected objects: ${objs.length}`);
let count = 0;
for (const obj of objs) {
    if (Math.random() > 0.5) {
        obj.selected = false;
        count += 1;
    }
}
print(`Deselected ${count} objects`);
