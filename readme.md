## PANKU

Panku is a header-only library designed to help with static initialisation of 
multiple devices, in cases where there are complex order dependencies between
them.

As an embedded project grows, there is a pattern that keeps repeating itself:
a monolithic file, usually named some variant of "InitialiseDevices", which 
containsall the code necessary to construct and interconnect elements of the 
system of varying abstraction levels, such as the scheduler, terminal, UART and 
Flash drivers, etc.

This file can easily grow out of control and become a maintenance burden. 
Furthermore, anyone wanting to add a new object to the static initialisation 
list needs to figure out exactly where to put it, which is time consuming.

Panku aims to solve that issue by allowing the user to explicitly state these
object dependencies, solving the dependency graph at compile time. Panku is not
a dependency injection framework, it does its job without invading the codebase.

## How to use
To integrate Panku in your project, include the file "Panku.h" and use the
following two macros:
*  **PANKU_LIST**: Declares the list of classes that Panku will be in charge
   of initialising. Each class is either marked as standalone (no dependencies)
   or as a dependency list, followed by the classes it depends on. Example:
   ```c++
   #include "Panku.h"
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

   This macro should be in a visible header file. Any code including that file
   will have access to the "panku" class, which has one member function, "Get".
   ```c++
   panku.Get<Flash>(); // Returns a reference to the Flash instance
   ```
