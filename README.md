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
# Table of contents
- [What is Cotton](#whatiscotton)
- [Why Cotton is bad](#whycottonisbad)
- [Why Cotton is good](#whycottonisgood)
- [Installation](#installation)
- [Documentation](#documentation)
- [Tests](#tests)
- [Usage](#usage)
- [Modules](#modules)
- [Source](#source)
- [For other learners](#forotherlearners)
- [Interesting moments](#interestingmoments)
- [Personal thoughts](#personalthoughts)

## What is Cotton <a name="whatiscotton"></a>
Cotton is a small programming language project that was made for the learning purposes during summer of 2024. The language was designed with simplicity in mind, having a rather easy syntax combination of Python and Go.

Cotton features a working garbage collector, as well as a complete system of builtin types and functions. Programs can be extended with modules which can be written in Both Cotton and C++ (using the cotton_lib library). 

Objects in the language can represent data and data types (which are called instance objects and type objects respectively).

At the momment, Cotton has 8 builtin types (Boolean, Character, Integer, Real, String, Nothing, Function, Array). Custom types (records) are also supported. 
The builtin functions contain a few dozens of functions essential to any interpreted programming language.

Cotton can load modules relatively both to the source file and to the COTTON_CTN_MODULES_PATH environment variable. Shared libraries written for Cotton can also be loaded.

## Why Cotton is bad <a name="whycottonisbad"></a>
Cotton is a learning project. It is not meant to be used in the real life. 

Cotton is incredibly slow (10 times slower than Python on simple loops). Parts of the language are untested and contain a lot of bugs (I hope to fix all of them till the end of summer). 

Not a lot of builtin stuff is supported in the language. You'd have to start from scratch if you wanted to write some serious programs in Cotton.

## Why Cotton is good <a name="whycottonisgood"></a>
Cotton supports modules which could negate the lack of things builtin into the language. 

The syntax is not very difficult, and a few good things like creating function objects make Cotton feel a bit better. 

## Installation<a name="installation"></a>
To make Cotton work, you have to build it first. 

Go to the root directory of the project. If you're building for the purpose of testing Cotton by writing some Cotton programs, uncomment the line with `O3` and `march=native` optimizations in the `CMakeLists.txt`.

Create the build directory and cd into it: 
```bash 
mkdir build; cd build
```

Run cmake, and then run make:
```bash
cmake .. && make
```

Now you should have a few directories created. The important ones are `cotton_int/`, `cotton_lib/`, and `cotton_modules/`. Usage of those directories will be described in the latter sections.


## Documentation <a name="documentation"></a>
To generate doxygen documentation of the source code, go to the source directory of the project, and simply run Doxygen:
```bash
doxygen
```
Documentation will be created in the `docs/` directory. To view it, open `docs/html/index.html` with a web browser.

Programming guide wil be available in the `cotton_guide/` directory.

Guide for developing in C++ for Cotton  will be available in the `cotton_lib_guide/` directory.

Builtin modules have their own documentation in the `cotton_modules_docs/` directory.

## Tests <a name="tests"></a>
Cotton has a suite of tests meant to verify that the language works correctly.

To run them, you need to have `python3` installed.

Go to the `tests/runtime_tests/` directory, and simply run:
```bash
python3 run_tests.py
```

## Usage<a name="usage"></a>
After building Cotton, you will need to add two environment variables if you want modules to work.

For builtin modules written in Cotton, set environment variable `COTTON_CTN_MODULES_PATH` equal to the directory `cotton_modules/` (make it absolute)

Some of the builtin modules need to load C++ shared libraries. Set environment variable `COTTON_SHARED_LIBRARIES_PATH` equal to the directory `build/cotton_modules/` (make it absolute)

Now, the `build/cotton_int/` directory contains an executable called `cotton_int` which is an interpreter for Cotton. To run a program, simply run the interpreter and pass a filename which will be interpreted as a Cotton program:
```bash
build/cotton_int/cotton_int tests/helloworld.ctn
```
You should see a message `Hello world!` appear in the terminal. 
Now simply pass any other file that you want instead of `tests/helloworld.ctn`, and you're good to go.

Cotton interpreter supports a few flags which can be passed alongside with the program filename.
- `--time` will print execution time of the program.
- `--disable_gc` will disable the garbage collector. *Don't use this one unless you want to crash the program intentionally :3*
- `--print_result` will print the object returned by the program.


## Modules <a name="modules"></a>
Currently only two modules are supported.
The first module is `gc`. It gives some access to garbage collector, and can be used to disable, enable,, and ping the gargabe collector. 

The second module is `helloworld`. It returns a string `"hello world"`.
 
Hopefully I will add more modules in the future.

## Source <a name="source"></a>
The source is divided into several directories.

- `cotton_in/` contains the interpreter code.

- `cotton_lib/` contains the code of the cotton library, which is responsible for tokenization, parsing, and execution of the code.

- `cotton_modules/` contains the code for the few builtin modules that Cotton has.

- `cotton-lang/` contains a vs code extension that provides extremely simply syntax highlighting for Cotton.

- `tests/` contains the test suit for Cotton.

Now, the `cotton_lib/` has a few subdirectories and files.

- `src/back/` contains files that implement the runtime of the language, i.e. the back end.

    - `api.h` contains the entire API of the back end.
    - `gc.h` contains headers for the garbage collector and gc strategies.
    - `instance.h` contains the instance abstract class, which represents data of objects.
    - `nameid.h` contains the system that assigns numbers to strings, and is used almost everywhere where strings are.
    - `object.h` contains the object class, which represent everything in Cotton.
    - `runtime.h` contains the Cotton runtime, which is basically a system that is responsible for executing Cotton code.
    - `scope.h` contains headers for scopes, which are used to get access to variables.
    - `type.h` contains the type abstract class, which are present in every object and represent its type.
- `src/builtin/` contains code for the builtin types and functions available in Cotton.
    - `functions/` contains the API header and code for the builtin functions like `make()` and `isoftype()`.
    - `types/` contains implementations of builtin types like `Integer` and `String`. The `record.h` is used when defining types in Cotton using the `type` construct.
- `src/front/` contains implementation of the front part of the language, namely the lexer and the parser.
- `src/errors.h` has API of the errors, which are used literally everywhere.
- `src/profiler.h` has simple declarations of a simple profiler which can be enabled with a certain compilation flag.
- other files are not that very much important to describe here.


## For other learners <a name="forotherlearners"></a>
Cotton is first of all a learning project which helped me realize how programming languages are made. I tried to describe how Cottonl libary works in  `cotton_lib_guide/`. If you want to ask any questions related to Cotton, I will be glad to answer them. Feel free to contact me via any existant method.

## Interesting moments <a name="interestingmoments"></a>
Cotton is pretty standard in terms of syntax and everything. However, during the initial designing faze I came up with a few things that I've never seen before, and I'd like to describe them here

You know how compiled programming languages have different passing modes for variables? Like in C++, the following two codes are different:

```C++
void foo(std::vector<int> array) ...
```
and
```C++
void foo(std::vector<int> &array) ...
```

The first function will accept a copy of the passed array, and the second one will accept the passed array without creating any copies of it.

Obviously, passing an item by reference (as in the second example) is much faster, since no copy is created. 

In python, for example, you can't specify the mode in which you want to pass an argument to a function. Simple types are passed by value (copied), and complex types are passed by reference (not copied). 

I didn't like that you couldn't simply pass an integer and modify it. The same thing is in Java by the way, where simple types are only passed by value.

Therefore, I made an operator `@`. The purpose of this operator is to specify that a value must not be copied ONCE. 

It means that doing `foo(a)` will accept a copy of `a`, while `foo(@a)` will accept the original value a.

What's more is that returning a value can also be done via the `@` operator. 
```js
method get(self) {
    return @self.x;
};
```

Imagive a class instance `obj` with a method `get` as presented above. If we wanted to increment its field `x`, we could do `obj.get()++`, and it would be exactly the same as doing `obj.x++` because of the `@` operator.

Another usage of `@` is at making variable references. For example:
```js
x = 5;
y = x;
y++; println(x); // prints 5
```

However,
```js
x = 5;
y = @x;
y++; println(x); // prints 6
```

This happens because assigning `y` to `@x` will make a new object with the same instance and type that `x` has. Therefore, `y` will be a different object, but will reference the exact same instance as does `x`.

...

This idea could be expanded further. What if a certain operator was used to actually make a copy of the object? Therefore, passing by value to a function would need that operator, but passing by reference wouldn't.

What if there were operators both for passing by value and by reference? Sounds pretty interesting.

## Personal thoughts <a name="personalthoughts"></a>
First of all, I want to write how Cotton became a reality:

So, during my first university semester, we had one course called "intro to the computer science". In approximately november 2023, we had a very interesting subject mentioned on that course.

The subject was about a stack based machine that was used to evaluate expressions. Later, we learned that such a machine could be used to execute programs with loops, ifs, and simple functions. That's when an idea had occured in my mind. "Hell, why don't I try to actually implement that machine?"

I spent an entire day implementing it. It was weird, but in the end, it worked, and i was able to print "hello world" using it, as well as to write some simple loops like fibonacci numbers.

That was when I realized that making a programming language was no longer impossible for me. It was hard, extremely hard, but possible. 

... (a few weeks later) ...

The winter exams had ended, and I had a few weeks of holidays. A fun thought visited my mind.

"hey, I don't know how to parse syntax of a complex programming language like C....... what if I didn't have to parse something as complex? What if my language had.... let's say.... keywords before each instruction? that was I would be able to parse every single instruction correctly, and I could actually make it work".

That was how my second project about programming languages was begun. I had an idea of making all instructions in the language of form (keyword arg1 arg2 arg3). For example, (add 1 5), (call print "hello world"), (set x (add x 1)). It was relatively easy to parse such a syntax. 

I liked the idea of a language being very simple (like Lua). I also had a lot of brackets in my language. So, I decided to call it Brua2 (BRackets + lUA = BRUA). Why 2? I don't remember actually. I think it was because it was my second project related to programming languages. Or maybe there was Brua already.

Fun thing about Brua2 was that I realized that there already was a programming language with a similar idea of using brackets, namely Lisp. 

I finished Brua2 in about 2-3 weeks. It was working, but it was done very unprofessionally, and I felt as if I could do more.

That's when I decided to experiment with parsers. I didn't know how formal grammars worked, but I had a general understanding of it. So, I tried implementing such a parser, with syntax very similar to the syntax of Cotton. 

It was a success. The parser worked, and it did so quite well. Better than I expected actually. That motivated me to continue developing that "project", so I began writing some very simply runtime. (Un)fortunatelly, I didn't have much time for that, since the semester had already started.

But the idea of writing a more complex programming language never left me. It was growing strong, and I began thinking about it almost immidiatelly after the semester started. I spent a lot of time thinking, designing, imagining, and simply developing a sketch of my new programming language.

At the beginning, it looked more like an improved version of Brua2: better syntax, better runtime. But it was still awful. So I kept thinking.

When the semster ended, I began actually writing some .idea files for my language. I decided to call it Cotton because it was supposed to be as easy and lightweight as is cotton in real life. (unfortunatelly, at the time I didn't know that a lot of projects already had that name).

I've spent hundreds of human hours working on Cotton. Designing it, writing code, testing it, rewriting it again and again. I've changed so many things so many times that I can't even count them anymore. 

So, summing it all up. Cotton was worth all the work I've put into it. It is a huge project that made me learn a lot of stuff about computer science and software engineering. My code became much clearer. I learned a few usefull tools along the way too. Basically, I became a better programmer.


