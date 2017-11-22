#include "Devices.h"
#include <iostream>

panku devices;

int main(void) {
   // This is optional. In its absence, user classes will be initialised
   // lazily during the first call to Get.
   devices.Initialise(); 

   devices.Get<Alpha>().Talk();
   devices.Get<Beta>().Talk();
   devices.Get<Gamma>().Talk();

   std::cout << "Now we will test iteration by parent class, for Alpha and Beta" << std::endl;
   // It is possible to iterate by base or derived class as well.
   devices.ForEach<AlphaBetaParent>([](AlphaBetaParent& parent) {
      parent.Talk();
   });
}
