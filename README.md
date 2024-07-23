# Cotton - a dynamically typed interpreted programming language
_A program in Cotton that checks if number 17 is prime_

```js
function check(n) {
    if n <= 1; return false;
    for i = 2; i * i <= n; i++; {
        if n % i == 0; return false;
    }
    return true;
};

num = 17;
println("Number", num, "is prime:", check(num));
```
---

Cottot is my second programming language, which has been drastically improved and optimized relatively to my first language [Brua2](https://github.com/lis05/Brua2).

## Features

The syntax of Cotton resembles that of C and Golang. 

A few builtin types are supported, namely `Boolean`, `Character`, `Integer`, `Real`, `String`, `Array`, `Function`, and `Nothing`. New data types can be defined both in Cotton (using the `type` expression) and in C++(using the internal Cotton API).

All types in Cotton can define methods which can be used via the `.` operator. Therefore, expressions like `object.method(args)` are supproted. Methods are shared between all the objects of the same type. Types defined via the `type` expression can specify what methods if that type will be available.

All objects may have fields which unlike methods are unique to each object. Types defined via the `type` expression can specify what fields objects of that type wil have.

The builtin functions in Cotton use methods when operating on the objects. For example, the `print` function will call the `__repr__` method in order to get the string representation of the objects. More about such "special" (or magic) methods will be in the documentation.

Cotton's builtin library contains functions that are essential to any program. Example of such functions are `make`, `copy`, `println`, `int`, `exit`, `typeof`, `hasfield`, `argv`. More about these functions will be in the documentation.

Cotton itself is a library that can be used to created extensions in C++. These extensions can be loaded into the program via the `load` function (**NOT IMPLEMENTED YET**). The same function can also be used to load (include) other programs in Cotton.