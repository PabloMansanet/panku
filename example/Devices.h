#pragma once

#include "panku.h"
#include "ExampleDeviceClasses.h"

PANKU_LIST
(
   INSTANCE(Beta, Alpha),
   INSTANCE(Alpha),
   INSTANCE(Gamma, Alpha, Beta),
   COLLECTION(2, NamedObject)
);

extern panku devices;
