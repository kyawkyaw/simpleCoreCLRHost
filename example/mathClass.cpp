#include "mathClass.h"

mathClass::mathClass(int value) : value(value) {}

void mathClass::doubleMe() {
  value = value*2;
}

int mathClass::getValue() {
  return value;
}
