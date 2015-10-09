#pragma once

class mathClass {
private:
  int value;
public:
  mathClass(int value);
  void doubleMe();
  int getValue();
};

extern "C" mathClass* mathClass_mathClass(int value) {
  return new mathClass(value);
}

extern "C" void mathClass_doubleMe( mathClass* m ) {
  m->doubleMe();
}

extern "C" int mathClass_getValue( mathClass* m ) {
  return m->getValue();
}
