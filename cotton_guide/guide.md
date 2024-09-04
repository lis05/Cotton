# Cotton programming guide

In Cotton, everything is an Object that stores an Instance and a Type.

- Instances hold the actual data (like numbers, strings, arrays).
- Types contain methods that can be used on the instance.

You can think of an object as of a "shell" that references the actual data. Therefore, multiple objects can refenrece the same data.

---

In Cotton, all statements are required to be followed by a semicolon.

Literals in Cotton are objects that cannot be assigned a different value. However, they can be modified, resulting in funny examples:
```js
// prints 2 == 3
if ++2 == 3; println("2 == 3");
```

---

Making variables requires that you assigned them some value immidiatelly:
```js
x = 5;
y = x + 5;
```
Making a variable this way creates a new object which is a direct copy of the righthand argument.

Usually, when an object is copied, its instance is being copied, but its type stays the same, meaning that a new object will be of the exact same type but with a different data. However, this is not true for user-defined data types, and it highly depends on the implementation.

---

Assigning values 