### PANKU

Panku is a header-only library designed to help with static initialisation of 
multiple devices, in cases where there are complex order dependencies between
them.

As an embedded project grows, there is a pattern that keeps repeating itself:
a monolithic file, usually named some variant of "InitialiseDevices", which 
contains all the code necessary to construct and interconnect elements of the 
system of varying abstraction levels, such as the scheduler, terminal, UART and 
Flash drivers, etc.

This file can easily grow out of control and become a maintenance burden. 
Furthermore, anyone wanting to add a new object to the static initialisation 
list needs to figure out exactly where to put it, which is time consuming.

Panku aims to solve that issue by allowing the user to explicitly state these
object dependencies, solving the dependency graph at compile time. Panku is not
a dependency injection framework, it does its job without invading the codebase.

## How to use
To integrate Panku in your project, include the file "panku.h" and use the
following macro:
*  **PANKU_LIST**: Declares the list of classes that Panku will be in charge
   of initialising. Each class is either marked as standalone (no dependencies)
   or as a dependency list, followed by the classes it depends on. Example:
   ```c++
   #include "panku.h"
   #include "UART.h"
   #include "Flash.h"
   #include "FileSystem.h"
   #include "Terminal.h

   PANKU_LIST
   (
      DEPENDENCY(FileSystem, Flash),    // File system depends on a flash driver
      STANDALONE(Flash),                // Flash driver has no dependencies
      DEPENDENCY(Terminal, UART, Flash),// Terminal depends on both drivers
      STANDALONE(UART)                  // UART is also dependency free
   );
   ```
   Note that this list can be defined in any order, the dependency graph will
   be solved at compile time. In case of a circular dependency, a static_assert
   will stop the program from compiling and prevent runtime undefined behaviour.

   What this macro does is define a class named "panku", that you can instantiate
   once in one of your translation units. Panku provides two methods, "Initialise"
   and "Get". Initialise performs the construction of all static objects, while
   Get retrieves a reference for you to use. It's possible to skip "Initialise",
   in which case it will be called lazily in the first use of "Get".
   ```c++
   panku.Get<Flash>(); // Returns a reference to the Flash instance
   ```
   **Important:** The Panku object must be unique!

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
The Panku algorithm works at a type level, so it can't easily reason about 
multiple instances of the same class. If you require several instances of the
same object, a solution is to wrap all of them under a composite object and 
add that to the PANKU_LIST.
