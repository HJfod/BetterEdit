const fs = require('fs');
const path = require('path');

let file_count = 0;
let dir_count = 0;
let header_count = 0;
let source_count = 0;
let resource_count = 0;
let line_count = 0;
let comment_count = 0;
let hook_count = 0;
let patch_count = 0;
let space_count = 0;
let char_count = 0;
let largest_count = 0;
let larget_line_count = 0;
let largest_file = "";
let class_count = 0;
const letter_count = {};

const rd = (dpath) => {
    const dir = fs.readdirSync(dpath);

    dir.forEach(file => {
        let is_code = false;
        switch (file.split('.').pop()) {
            case 'hpp': case 'h': header_count++; is_code = true; break;
            case 'cpp': case 'c': source_count++; is_code = true; break;
            case 'png': case 'jpg': resource_count++; break;
        }

        if (!path.join(dpath, file).includes('.git')) {
            try {
                if (fs.statSync(path.join(dpath, file)).isDirectory()) {
                    rd(path.join(dpath, file));

                    dir_count++;
                } else throw "bruh";
            } catch {
                file_count++;
                
                if (is_code) {
                    const data = fs.readFileSync(path.join(dpath, file)).toString();

                    line_count += data.split('\n').length;
                    char_count += data.split('').length;
                    space_count += data.match(/\s/g).length;

                    if (largest_count < data.split('').length) {
                        largest_count = data.split('').length;
                        larget_line_count = data.split('\n').length;
                        largest_file = file;
                    }

                    data.split('\n').forEach(f => {
                        if (f.trim().startsWith('//'))
                            comment_count++;
                        else {
                            if (f.trim().startsWith('GDMAKE_HOOK'))
                                hook_count++;
                        
                            patch_count += f.includes("patch");
                            patch_count -= f.includes("unpatch");

                            class_count += f.includes("class");
                            class_count += f.includes("struct");
                        }
                        
                        f.split('').forEach(c => {
                            if (!letter_count[c])
                                letter_count[c] = 0;
                            letter_count[c]++;
                        });
                    });
                }
            }
        }
    });
}

rd(__dirname);

const r = '\x1b[31m';
const g = '\x1b[32m';
const y = '\x1b[33m';
const b = '\x1b[34m';
const c = '\x1b[36m';
const p = '\x1b[35m';
const n = '\x1b[0m';

let most_common = '';
let most_common_l = '';
for (const [key, value] of Object.entries(letter_count)) {
    if (!most_common || value > letter_count[most_common])
        most_common = key;

    if ((!most_common_l || value > letter_count[most_common_l]) && key != ' ')
        most_common_l = key;
}

console.log();
console.log(`${g} >> BetterEdit Stats >> ${n}`);
console.log();
console.log(`${y}${file_count}${n} files (
${p}${header_count}${n} headers, 
${c}${source_count}${n} source files, 
${r}${resource_count}${n} resources, 
${g}${file_count - header_count - source_count - resource_count}${n} other, 
${b}${dir_count}${n} directories)
`.replace(/\n/g, ''));
console.log(`${p}${line_count}${n} lines of code (${g}${Math.round(comment_count / line_count * 100)}%${n} comments)
`.replace(/\n/g, ''));
console.log(`Largest file is ${b}${largest_file}${n} (${r}${largest_count}${n} chars, ${g}${larget_line_count}${n} lines)
`.replace(/\n/g, ''));
console.log(`Average line count per file is ${c}${Math.round(line_count / file_count)}${n}
`.replace(/\n/g, ''));
console.log(`${g}${char_count}${n} characters (${y}${Math.round(space_count / char_count * 100)}%${n} whitespace)
`.replace(/\n/g, ''));
console.log(`${r}${hook_count}${n} hooks
`.replace(/\n/g, ''));
console.log(`${y}${patch_count}${n} patches
`.replace(/\n/g, ''));
console.log(`${p}${class_count}${n} classes
`.replace(/\n/g, ''));
console.log(`Most common character is ${c}'${most_common}'${n} (${b}${letter_count[most_common]}${n} occurences)`);
console.log(`Most common letter is ${r}'${most_common_l}'${n} (${g}${letter_count[most_common_l]}${n} occurences)`);
console.log();
