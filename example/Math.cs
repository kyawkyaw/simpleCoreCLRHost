using System;

public class Math {
  public static unsafe void DoubleIt(int *x) {
    Console.WriteLine("Here's C# code! I'm doing some math.");
    *x = *x * 2;
  }
}
