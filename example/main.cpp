#include "Devices.h"
#include <iostream>
#include <tuple>

panku_one devices;
panku_two devices2;

panku notypename;

using namespace ::std;

int main(void) {
   // This is optional. In its absence, user classes will be initialised
   // lazily during the first call to Get.
   devices.Initialise(); 

   devices.Get<Alpha>().Talk();
   devices.Get<Beta>().Talk();
   devices.Get<Gamma>().Talk();

   cout << "Now we will test entry collections (in this case, two different instances of NamedObject);" << endl;

   devices.Get<NamedObject,0>().Talk();
   devices.Get<NamedObject,1>().Talk();

   cout << "Now we will test iteration by parent class, for Alpha, Beta and the named objects in a collection" << endl;
   // It is possible to iterate by base or derived class as well.
   devices.ForEach<MultiParent>([](MultiParent& parent) {
      parent.Talk();
   });

   devices2.Initialise(); 
   devices2.Get<Delta>().Talk();
   devices2.Get<Epsilon>().Talk();
   devices2.Get<Zeta>().Talk();
   devices2.Get<Eta>().Talk();

   cout << "Now we will create a simple panku that has no distinct type name. This use case is maintained for backward compatibility" << endl;
   notypename.Initialise();
   notypename.Get<Alpha>().Talk();
   notypename.Get<Zeta>().Talk();
}
