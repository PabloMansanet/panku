#include "Devices.h"

panku devices;

int main(void) {
   // This is optional. In its absence, user classes will be initialised
   // lazily during the first call to Get.
   devices.Initialise(); 

   devices.Get<Alpha>().Talk();
   devices.Get<Beta>().Talk();
   devices.Get<Gamma>().Talk();
}
