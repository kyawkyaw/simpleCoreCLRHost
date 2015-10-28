using System;
using System.Runtime.InteropServices;

public class Managed {

unsafe delegate void myDelegate( IntPtr x );

public static unsafe void runIt( IntPtr obj, IntPtr mem_fun ) {
    Console.WriteLine("Here's C# code:");

    myDelegate fun = (myDelegate) Marshal.GetDelegateForFunctionPointer( mem_fun, typeof(myDelegate) );

    fun(obj);  // first argument of member functions in C++ is "this", but it's hidden from us :-)
  }
}
