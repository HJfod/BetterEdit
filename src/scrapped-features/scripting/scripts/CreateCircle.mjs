// @ts-check

print("hey");
const pos = editor.getViewCenter();
print(`pos: ${pos.x}, ${pos.y}`);
const pos2 = editor.getViewCenter("error");
print("yeah");
// setTimeout(() => {
//     print("hello");
// }, 0);
// print("hiii");

// const x = new Promise((resolve, _) => setTimeout(() => resolve("hiiii"), 3000));
// x.then(value => {
//     print(`got x: ${value}`);
// });

// const options = await input({
//     objID: {
//         "type": "int",
//         "name": "Object ID"
//     },
//     objCount: {
//         "type": "int",
//         "name": "Object count"
//     },
//     radius: {
//         "type": "number",
//         "name": "Radius",
//         "description": "Circle radius in units (30 units = 1 block)"
//     },
// });
// print(JSON.stringify(options, undefined, 4));

// const pos = editor.getViewCenter();

// let angle = 0;
// for (let i = 0; i < options.objCount; i += 1) {
//     const obj = editor.createObject(options.objID);
//     obj.x = pos.x + Math.cos(angle) * options.radius;
//     obj.y = pos.y + Math.sin(angle) * options.radius;
//     obj.rotation = 360 - angle;
//     angle += 360 / options.objCount;
// }
