#include <dlfcn.h>
#include "ers/ers.h"

#include "FaserEventStorage/fRead.h" 
#include "FaserEventStorage/loadfRead.h"

fRead * loadfRead(std::string libName){
 
  ERS_DEBUG(2,"loadfRead called with library name "<<libName);
 
  ::dlerror();
 
  libName += ".so";
  libName = "lib" + libName;

  void * lib = ::dlopen(libName.c_str(), RTLD_NOW | RTLD_GLOBAL);

  if (!lib || ::dlerror() ) {
    ERS_DEBUG(1,"Cannot load library");
    return NULL;
  }

  ERS_DEBUG(2,"Library OK, now find the symbol.");

  ::dlerror();

  // fun is defined to be: 
  // a pointer to a function which takes no arguments "()" 
  // and returns fRead* (e.g., look in fReadPlain fReadFactory)
  typedef fRead* (*fun)(); 
  fun sym = (fun)::dlsym(lib, "fReadFactory");

  if (!sym || ::dlerror()) {
    ERS_DEBUG(1,"No symbol");
    return NULL;
  }
  ERS_DEBUG(2,"Symbol is OK, now get the reader.");

  return sym();   // this returns an "fRead*"
}
