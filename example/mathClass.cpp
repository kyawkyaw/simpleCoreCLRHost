class mathClass {
private:
  int value;
public:
  mathClass(int value) : value(value) {}
  void doubleMe() { value = value*2; }
  int getValue() { return value; }
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
