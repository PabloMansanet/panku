#include "ExampleDeviceClasses.h"
#include <cstdio>

Alpha::Alpha()
{
   printf("Hi! I'm Alpha and I'm being initialised. I don't know anyone yet!\n\n\n");
}

void Alpha::Talk()
{
   printf("Hi! I'm Alpha\n");
}

void Beta::Talk()
{
   printf("Hi! I'm Beta, and the meaning of life is %d\n", meaningOfLife);
}

Beta::Beta(Alpha& alpha):
   meaningOfLife(42)
{
   printf("I'm Beta reading Alpha on initialisation, say hi, Alpha:\n");
   alpha.Talk();
   printf("\n\n");
}

void Gamma::Talk()
{
   printf("Hi! I'm Gamma, and the meaning of life is %d\n", meaningOfLife);
}

Gamma::Gamma(Alpha& alpha, Beta& beta):
   meaningOfLife(42)
{
   printf("I'm Gamma reading Alpha and Beta on initialisation, say hi, Alpha and Beta:\n");
   alpha.Talk();
   beta.Talk();
   printf("\n\n");
}

Delta::Delta()
{
   printf("Hi! I'm Delta and I'm being initialised. I don't know anyone yet!\n\n\n");
}

Epsilon::Epsilon()
{
   printf("Hi! I'm Epsilon and I'm being initialised. I don't know anyone yet!\n\n\n");
}

Zeta::Zeta()
{
   printf("Hi! I'm Zeta and I'm being initialised. I don't know anyone yet!\n\n\n");
}

Eta::Eta(Zeta& zeta)
{
   printf("I'm Eta reading zeta on initialisation, say hi, Zeta:\n");
   zeta.Talk();
   printf("\n\n");
}


NamedObject::NamedObject(const char* name):
   name(name)
{
}

void NamedObject::Talk()
{
   printf("I'm a named object, and my name is %s\n", name);
}

void Delta::Talk()
{
   printf("Hi! I'm Delta\n");
}

void Epsilon::Talk()
{
   printf("Hi! I'm Epsilon\n");
}

void Zeta::Talk()
{
   printf("Hi! I'm Zeta\n");
}

void Eta::Talk()
{
   printf("Hi! I'm Eta\n");
}


