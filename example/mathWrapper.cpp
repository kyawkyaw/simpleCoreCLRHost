#include "mathClass.h"

extern "C" mathClass* mathClass_mathClass(int value) {
  return new mathClass(value);
}

extern "C" void mathClass_doubleMe( mathClass* m ) {
  m->doubleMe();
}

extern "C" int mathClass_getValue( mathClass* m ) {
  return m->getValue();
}
