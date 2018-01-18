#include "Devices.h"
#include <iostream>
#include <tuple>

panku devices;

typedef PankuMetaprogram::Collection<NamedObject*, 3> Col;

int main(void) {
   static_assert(PankuMetaprogram::is_collection<Col>::value);
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

   std::cout << "Now we will test entry collections (in this case, two different instances of NamedObject);" << std::endl;

   devices.Get<NamedObject,0>().Talk();
   devices.Get<NamedObject,1>().Talk();



}
