#pragma once

#include "named_panku.h"
#include "panku.h"
#include "ExampleDeviceClasses.h"

NAMED_PANKU_LIST
(
   one,
   NAMED_INSTANCE(Beta, Alpha),
   NAMED_INSTANCE(Alpha),
   NAMED_INSTANCE(Gamma, Alpha, Beta),
   NAMED_COLLECTION(2, NamedObject)
);

NAMED_PANKU_LIST
(
   two,
   NAMED_INSTANCE(Delta),
   NAMED_INSTANCE(Epsilon),
   NAMED_INSTANCE(Zeta),
   NAMED_INSTANCE(Eta, Zeta)
);

PANKU_LIST
(
   NAMED_INSTANCE(Alpha),
   NAMED_INSTANCE(Zeta)
);

extern panku_one devices;
extern panku_two devices2;
extern panku noname;


