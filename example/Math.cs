using System;
using System.Runtime.InteropServices;

public class Math {

  [DllImport("../SCCH")]
  static public extern IntPtr mathClass_mathClass(int value);

  [DllImport("../SCCH")]
  static public extern unsafe void mathClass_doubleMe( IntPtr i );

  [DllImport("../SCCH")]
  static public extern int mathClass_getValue( IntPtr i );

  public static unsafe void DoubleIt(int *x) {
    Console.WriteLine("Here's C# code! I'm doing some math using C++ class.");

    IntPtr m = mathClass_mathClass(*x);
    mathClass_doubleMe(m);

    *x = mathClass_getValue(m);
  }
}
