#include "ExampleDeviceClasses.h"
#include <cstdio>

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
   printf("I'm Beta reading Alpha on initialization, say hi, Alpha:\n");
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
   printf("I'm Gamma reading Alpha and Beta on initialization, say hi, Alpha and Beta:\n");
   alpha.Talk();
   beta.Talk();
   printf("\n\n");
}

