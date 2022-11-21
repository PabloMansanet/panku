#pragma once

#include "named_panku.h"
#include "ExampleDeviceClasses.h"

PANKU_LIST
(
   one,
   INSTANCE(Beta, Alpha),
   INSTANCE(Alpha),
   INSTANCE(Gamma, Alpha, Beta),
   COLLECTION(2, NamedObject)
);

PANKU_LIST
(
   two,
   INSTANCE(Delta),
   INSTANCE(Epsilon),
   INSTANCE(Zeta),
   INSTANCE(Eta, Zeta)
);

extern panku_one devices;
extern panku_two devices2;
