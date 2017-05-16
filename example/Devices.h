#pragma once

#include "Panku.h"
#include "ExampleDeviceClasses.h"

PANKU_LIST
(
   DEPENDENCY(Beta, Alpha),
   STANDALONE(Alpha),
   DEPENDENCY(Gamma, Alpha, Beta)
);

extern Panku panku;
