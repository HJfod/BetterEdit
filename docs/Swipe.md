# Swipe

**Swipe** is a purpose-built interpreted scripting language for BetterEdit. Its syntax is a mash between JavaScript and Rust, and it has been built to be simple and easy to understand.

## Declaring variables

Swipe is dynamically typed, so no type annotations are needed. Swipe also doesn't have a variable declaration keyword, and instead variables automatically come into existence the first time their name is referenced:

```js
a = 5
b = "string value"
c = [1, 2, 3]
```

## Semicolons

Semicolons are not required in Swipe, however they are allowed and can be used to disambiguate certain situations.

Swipe's parser by default matches greedily and ignores all whitespace; in other words, if an expression could match longer, it will. This means that something like: 

```rust
a = 5

+ 2


   + 3
```

will get matched as a single statement `a = 5 + 2 + 3`. In most cases, this is fine, as the most greedily matched tree is the one intended by the programmer, but in some situations semicolons may be used to cut the matching short:

```rust
a = 5;
+ 2
// gets parsed as two statements: `a = 5` and `+2`
```

If so desired, semicolons can be made obligatory by using `@strict`:

```rust
@strict true

a = 5
b = 2 // error: semicolon expected
```

## If statements

If statements follow the standard pattern of `if` keyword followed by a condition and a block of code, and then optionally an `else` keyword followed by another if statement or block of code.

If statements don't require parenthesis, however they may be used if you so wish to.

```rust
if x > 5 {
    // something new
}
else {
    // something blue
}
```

## Functions

Functions can be declared using the `function` keyword:

```js
function awesome(a, b) {
    return a + b;
}

awesome(2, 7);
```

## Statements & expressions

All statements in Swipe are expressions, meaning that you can use constructs like If statements to assign values:

```rust
x = if condition {
    "value"
}
else {
    "other"
}
```

By default, a block of code evaluates to the last value in the block, or `null` if the block is empty:

```rust
x = {
    "value"
    "other"
}
assert(x == "other")

x = {}
assert(x == null)
```

## Comments

Line comments in Swipe are declared with `//` and block comments with `/*`:

```rust
// this is a comment
/* 
 this is also a comment
*/
```

## Metadata

A script can have metadata at the start of the file, prefixed with `@`:

```rust
@version 1.0
@input number value
```

Valid metadata tags:

 * `@version <number>` - The version of Swipe the script was made for
 * `@input [type] <ident>` - A value that the user is prompted to provide before running the script. Possible types are `number` and `string`
 * `@strict [bool]` - Enable strict mode, which makes the parser require semicolons at the end of statements

A script can prompt the user for input before running
