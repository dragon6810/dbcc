# Deadbird Compiler Collection IR Specification

## Translation Unit

Translation units are composed of an arbitrary of `Declarations` and `Definitions`.

## Declaration

### Type Declaration

**TODO: Type Declarations**

### Global Variable Declaration

Global Variable declarations are denoted with the `declare` and `variable` keywords, followed by the type and name, usually preferably with the `#` character. For example for a variable declared by `float foo;` in C, it could be like this:

    declare variable f32 foo


### Function Declaration

Function declarations are denoted with the `declare` and `function` keywords, followed by the return type and then the function's name, preferably prefixed with an `@` character. It is followed by a list of typed parameters. For example for a function declared by `int add(int a, int b);` in C, it could be like this:

    declare function i32 @add i32 a i32 b

## Definition

### Struct Definition

**TODO: Struct Definitions**

### Global Variable Definition

Global Variable definitions are denoted with the `define` and `variable` keywords, followed by the name. The name must be the name of an already declared Global Variable, and one Global Variable must not be defined more than once. Optionally, you can also include the default value for the variable. For example a Global Variable definition such as `int bar = 3;` in C could look like:

    define variable #bar 3

### Function Definition

Function Definitions are denoted with the `define` and `function` keywords, followed by the name. The name must be the name of an already declared Function definition, and one Function must not be defined more than once. A Function definition must be followed by 1 or more instructions, including a return statement in all paths. For example the IR Function Definition of the C function:

    int myfun(int foo, float bar)
    {
        ...
    }

Could look like:

    define function @myfun i32 foo f32 bar
        ...

## Instruction

An instruction is an `opcode`, followed by a variadic number of parameters (seperated by commas), depending on both the `opcode` and context.

For example, a two parameter instruction could look like:

    opcode param1, param2

Below is a list of `opcodes` and their parameters

### ADD

The structure of the `ADD` opcode is the following:

    ADD Register dest, Value a, Value b

This will write a + b into register dest.

### SUB

The structure of the `SUB` opcode is the following:

    SUB Register dest, Value a, Value b

This will write a - b into register dest.

### MUL

The structure of the `MUL` opcode is the following:

    MUL Register dest, Value a, Value b

This will write a * b into register dest.

### DIV

The structure of the `DIV` opcode is the following:

    DIV Register dest, Value a, Value b

This will write a / b into register dest.

### CAS

The structure of the `CAS` opcode is the following:

    CAS Type desttype, Register dest, Type srctype, Register src

This will cast src into dest.

### LOD

The structure of the `LOD` opcode is the following:

    LOD Register dest, Value src

This will load the src value into dest.

### CMP

The structure of the `CMP` opcode is the following:

    CMP Value a, Value b

This will compare a to b and trigger any comparison flags

### JMP

The structure of the `JMP` opcode is the following:

    JMP Label label

This will jump to the given label

### JE

The structure of the `JE` opcode is the following:

    JE Label label

This will jump to the given label if the `equal` comparison flag is set

### JNE

The structure of the `JNE` opcode is the following:

    JNE Label label

This will jump to the given label if the `equal` comparison flag is not set

### JL

The structure of the `JL` opcode is the following:

    JL Label label

This will jump to the given label if the `less` comparison flag is set

### JG

The structure of the `JG` opcode is the following:

    JG Label label

This will jump to the given label if the `greater` comparison flag is set

### JLE

The structure of the `JLE` opcode is the following:

    JLE Label label

This will jump to the given label if the `less or equal` comparison flag is set

### JGE

The structure of the `JGE` opcode is the following:

    JGE Label label

This will jump to the given label if the `greater or equal` comparison flag is set

## Type

A type can be a primitive type, for example `i32` or `f64`, or a custom type for example `struct.mystruct` or `union.myunion`.

## Value

In the following description, `vn` is a constant value, and `rn` is the index of any register. `type` is any `Type`, for example `i32` or `struct.mystruct`. `in` is any integer.

A value can be stored in several modes:

    type v1 // A constant value

    type %r1 // The value currently in %r1

    type [i1] // The value in memory at i1

    type [type %r1] // The address at %r1

    type [type %r1 + type %r2] // The address at %r1 + %r2

    type [i1 + i2 * %r1 + i3] // If i2 is 1, it can be implicit. If i1 or i3 is 0, they can be implicit.