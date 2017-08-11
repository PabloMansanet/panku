#include "Devices.h"

// The following initialisation functions are defined here, but they could be
// anywhere in the source code, including in their own translation units.
template<>
Alpha& ConstructAndInitialise<Alpha&>() {
   static Alpha alpha;
   return alpha;
}

template<>
Beta& ConstructAndInitialise<Beta&>() {
   static Beta beta(devices.Get<Alpha>());
   return beta;
}

template<>
Gamma& ConstructAndInitialise<Gamma&>() {
   static Gamma gamma(devices.Get<Alpha>(), devices.Get<Beta>());
   return gamma;
}

