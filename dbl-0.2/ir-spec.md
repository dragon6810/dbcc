# Deadbird Compiler Collection IR Specification

## Translation Unit

Translation units are composed of an arbitrary of `Declarations` and `Definitions`.

## Declaration

### Type Declaration

**TODO: Type Declarations**

### Global Variable Declaration

Global Variable declarations are denoted with the `declare` and `variable` keywords, followed by the type and name, usually preferably with the `#` character. For example for a variable declared by `float foo;` in C, it could be like this:

    declare variable f32 #foo


### Function Declaration

Function declarations are denoted with the `declare` and `function` keywords, followed by the return type and then the function's name, preferably prefixed with an `@` character. It is followed by a list of typed parameters. For example for a function declared by `int add(int a, int b);` in C, it could be like this:

    declare function i32 @add i32 a i32 b

## Definition

### Struct Definition

**TODO: Struct Definitions**

### Global Variable Definition

Global Variable definitions are denoted with the `define` and `variable` keywords, followed by the type and name. The name must be the name of an already declared Global Variable, and one Global Variable must not be defined more than once. Optionally, you can also include the default value for the variable. For example a Global Variable definition such as `int bar = 3;` in C could look like:

    define variable i32 #bar 3

### Function Definition

Function Definitions are denoted with the `define` and `function` keywords, followed by the type and name. The name must be the name of an already declared Function definition, and one Function must not be defined more than once. A Function definition must be followed by 1 or more instructions, including a return statement in all paths. For example the IR Function Definition of the C function:

    int myfun(int foo, float bar)
    {
        ...
    }

Could look like:

    define function i32 @myfun i32 $foo f32 $bar
        ...

## Instruction

An instruction is an `opcode`, followed by a variadic number of parameters (seperated by commas), depending on both the `opcode` and context.

For example, a two parameter instruction could look like:

    opcode param1, param2

Below is a list of `opcodes` and their parameters

### add

The structure of the `add` opcode is the following:

    add Register dest, Value a, Value b

This will write a + b into register dest.

### sub

The structure of the `sub` opcode is the following:

    sub Register dest, Value a, Value b

This will write a - b into register dest.

### mul

The structure of the `mul` opcode is the following:

    mul Register dest, Value a, Value b

This will write a * b into register dest.

### div

The structure of the `div` opcode is the following:

    div Register dest, Value a, Value b

This will write a / b into register dest.

### cast

The structure of the `cast` opcode is the following:

    cast Type desttype, Register dest, Type srctype, Value src

This will cast src into dest.

### load

The structure of the `load` opcode is the following:

    load Register dest, Value src

    OR

    load Register dest, [Variable var + Integer offset]

This will load the src value into dest. If you take the second approach, and the offset is 0, it can be implicit.

### cmp

The structure of the `cmp` opcode is the following:

    cmp Value a, Value b

This will compare a to b and trigger any comparison flags

### jmp

The structure of the `jmp` opcode is the following:

    jmp Label label

This will jump to the given label

### je

The structure of the `je` opcode is the following:

    je Label label

This will jump to the given label if the `equal` comparison flag is set

### jne

The structure of the `jne` opcode is the following:

    jne Label label

This will jump to the given label if the `equal` comparison flag is not set

### jl

The structure of the `jl` opcode is the following:

    jl Label label

This will jump to the given label if the `less` comparison flag is set

### jg

The structure of the `jg` opcode is the following:

    jg Label label

This will jump to the given label if the `greater` comparison flag is set

### jle

The structure of the `jle` opcode is the following:

    jle Label label

This will jump to the given label if the `less or equal` comparison flag is set

### jge

The structure of the `jge` opcode is the following:

    jge Label label

This will jump to the given label if the `greater or equal` comparison flag is set

### call

The structure of the `call` opcode is the following:

    call Function function, Variable output, ...

This will call the given function, returning the value into output, and use all args after the output as the input parameters

### return

The structure of the `return` opcode is the following:

    return Variable value

    OR

    return

This will return the given value. If there is no value, it will return a `void`

### salloc

The structure of the `salloc` opcode is the following:

    salloc Variable $variable, Integer size

This will declare and allocate a variable on the stack.

## Type

A type can be a primitive type, for example `i32` or `f64`, or a custom type for example `struct.mystruct` or `union.myunion`.

## Value

In the following description, `cn` is a constant value, and `rn` is the index of any register. `type` is any `Type`, for example `i32` or `struct.mystruct`. `in` is any integer. `vn` is any variable.

A value can be stored in 7 modes:

    Constant:
    type c1 // A constant value

    Register:
    type %r1 // The value currently in %r1

    Direct Address:
    type [i1] // The value in memory at i1

    Indirect Address:
    type [type %r1] // The address at %r1

    Indirect Sum Address:
    type [type %r1 + type %r2] // The address at %r1 + %r2

    Offset Indirect Address:
    type [%r1 + i1 * i2 + i3] // If i2 is 1, it can be implicit. If i1 or i3 is 0, they can be implicit.

    Offset Variable Address:
    type [v1 + i1 * i2 + i3] // If i2 is 1, it can be implicit. If i1 or i3 is 0, they can be implicit.