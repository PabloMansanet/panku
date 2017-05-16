#include "Devices.h"

Panku panku;

int main(void) {
   // This is optional. In its absence, user classes will be initialised
   // lazily during the first call to Get.
   panku.Initialise(); 

   panku.Get<Alpha>().Talk();
   panku.Get<Beta>().Talk();
   panku.Get<Gamma>().Talk();
}
