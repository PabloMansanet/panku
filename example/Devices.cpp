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

template<>
Delta& ConstructAndInitialise<Delta&>() {
   static Delta delta;
   return delta;
}

template<>
Epsilon& ConstructAndInitialise<Epsilon&>() {
   static Epsilon epsilon;
   return epsilon;
}

template<>
Zeta& ConstructAndInitialise<Zeta&>() {
   static Zeta zeta;
   return zeta;
}

template<>
Eta& ConstructAndInitialise<Eta&>() {
   static Eta eta(devices2.Get<Zeta>());
   return eta;
}

template<>
NamedObject& ConstructAndInitialise<NamedObject&, 0>() {
   static NamedObject namedObjectZero("Zero");
   return namedObjectZero;
}

template<>
NamedObject& ConstructAndInitialise<NamedObject&, 1>() {
   static NamedObject namedObjectOne("One");
   return namedObjectOne;
}
