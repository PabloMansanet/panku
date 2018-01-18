#pragma once

#include "panku.h"
#include "ExampleDeviceClasses.h"

PANKU_LIST
(
   DEPENDENCY(Beta, Alpha),
   DEPENDENCY(Alpha),
   DEPENDENCY(Gamma, Alpha, Beta),
   COLLECTION(2, NamedObject)
);

extern panku devices;
