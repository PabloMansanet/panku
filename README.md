### PANKU

Panku is a header-only library designed to help with static initialisation of
multiple objects, in cases where there are complex order dependencies between
them.

It was created for an embedded use case. As an embedded project grows, there is 
a pattern that keeps repeating itself: a monolithic file, usually named some 
variant of "InitialiseDevices", which contains all the code necessary to construct 
and interconnect elements of the system of varying abstraction levels, such as the 
scheduler, terminal, UART and Flash drivers, etc.

This file can easily grow out of control and become a maintenance burden.
Furthermore, anyone wanting to add a new object to the static initialisation
list needs to figure out exactly where to put it, which is time consuming.

Panku aims to solve that issue by allowing the user to explicitly state these
object dependencies, solving the dependency graph at compile time. Panku is not
a dependency injection framework, it does its job without invading the
codebase.

## How to use

To integrate Panku in your project, include the file "named_panku.h"
and use the following macro:
*  **NAMED_PANKU_LIST**: Declares a panku name and the list of classes that panku 
   will be in charge of initialising. Each class is introduced as a dependency list, 
   followed by the classes it depends on. Example:
   ```c++
   #include "named_panku.h"
   #include "UART.h"
   #include "Flash.h"
   #include "FileSystem.h"
   #include "Terminal.h

   NAMED_PANKU_LIST
   (
      devices,
      NAMED_INSTANCE(FileSystem, Flash),     // File system depends on a flash driver
      NAMED_INSTANCE(Flash),                 // Flash driver has no dependencies
      NAMED_INSTANCE(Terminal, UART, Flash), // Terminal depends on both drivers
      NAMED_INSTANCE(UART)                   // UART is also dependency free
   );
   ```
   Note that this list can be defined in any order, the dependency graph will
   be solved at compile time. In case of a circular dependency, a static_assert
   will stop the program from compiling and prevent runtime undefined behaviour.

   What this macro does is define a class named "Panku", that you can declare
   and instantiate once in one of your translation units. Panku provides two
   methods, "Initialise" and "Get". Initialise performs the construction of all
   static objects, while Get retrieves a reference for you to use. It's
   possible to skip "Initialise", in which case it will be called lazily in the
   first use of "Get".
   ```c++
   panku_devices devices;
   devices.Get<Flash>(); // Returns a reference to the Flash instance
   ```
*  **Important:** As a panku grows, it takes more resources to solve the 
   "dependency puzzle". It is recommended that you split large pankus up to
   avoid excessive RAM and CPU use at compile-time.

Finally, even though Panku can figure out in which order to initialise your 
devices, it has no way to know what constructor arguments you want. To specify
how Panku should build each of the listed devices, you must define a function
with the following format for each of them (they can be defined anywhere):
```c++
template<>
MyClass& ConstructAndInitialise<MyClass&>() {
   static MyClass myObject;
   return myObject;
}
```

## Multiple object instances:
If you require multiple instances of the same class, you can use 
NAMED_COLLECTION(N, Class, Deps...) in place of NAMED_INSTANCE(Class, Deps...). 
You will have to supply N constructor functions of the form: 

```c++
Class& ConstructAndInitialise<Class&, 0>;
Class& ConstructAndInitialise<Class&, 1>;
...
Class& ConstructAndInitialise<Class&, N>;
```

These constructor functions will be called sequentially. To retrieve objects of
this type from the Panku class, you will have to call the templated form of Get:

```c++
devices.Get<Class&, 1>(); // Retrieves the second object of type "Class"
```

## Iterating over objects by their common parent:
Panku includes a very handy way of iterating over all elements who have a common
parent:

```c++
devices.ForEach<ParentType>([](auto& object) {
   object.DoSomething();
});
```

Iterations over objects with arbitrary type traits will be supported in the future
(i.e. "Do something to all classes with a trivial constructor");
