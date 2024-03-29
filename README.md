# simpleCoreCLRHost

  Disclaimer: This is only [PoC](https://en.wikipedia.org/wiki/Proof_of_concept), do not copy-paste this code into production.

  This C++ app allows to run custom C# method from compiled C# .dll on Linux and OS X using coreCLR.
  In our example that C# method runs C++ class methods on C++ objects using pointers and delegates.
  It's code is based on [example from coreCLR](https://github.com/dotnet/coreclr/tree/master/src/coreclr/hosts/unixcoreruncommon)
  and was done with big help from [**@janvorli**](https://github.com/janvorli), who patiently answered my questions.

## Prerequisites
  First, You need GCC 5.3+ or any other compiler that already supports C++ Filesystem TS.

  You need to have ICU 52 library in your linker path. If your Linux distribution have ICU 55 ( like Arch ), you can rebuild
  System.Globalization.Native.so  by yourself, or copy ICU 52 files from another distribution repository.

  You also need DNX SDK ( coreclr+corefx )
  ( [Linux](https://github.com/dotnet/coreclr/blob/master/Documentation/install/get-dotnetcore-dnx-linux.md), [OS X](https://github.com/dotnet/coreclr/blob/master/Documentation/install/get-dotnetcore-dnx-osx.md) )
  and *Mono* to build and run example.

## Compilation
  Just link with dl, and remember to set c++14 and libstdc++fs flags
  ( because coreCLR is not compatible with libc++ )

    g++ -std=c++14 -Wall -Wextra -Werror simpleCoreCLRHost.cpp -o SCCH -ldl -lstdc++fs
  You can use other name that *SCCH* for output, this is just for example.

## Usage


    ./SCCH DLL_PATH ASSEMBLY_PATH ENTRY_POINT_TYPE ENTRY_POINT_NAME

  *DLL_PATH* - **absolute** path to coreFX/coreCLR DLL dir ( with libcoreclr.so, mscorlib.dll, and other used by program ).

  *ASSEMBLY_PATH* - **relative** path to assembly to run.

  *ENTRY_POINT_TYPE* - EntryPoint type, often this will be it's class name ( check example ).

  *ENTRY_POINT_NAME* - method name to run from given assembly.

  **You will get 0x80131040 error with assembly is in DLL_PATH dir!** - this is due to TPA restrictions and this app being *simple*.

### Why we will get 0x80131040 error? What's the solution if I must have them in the same dir?
You will get 0x80131040 error, because your assembly will get listed in Trusted Platform Assemblies. And every requested assemby from TPA list must have valid version string. Most ootb mcs compiled programs have version 0.0.0.0, so In our example instead of requesting "Square" assembly app need to request "Square, Version=0.0.0.0" assembly ( I'm talking about executeAssembly's assemblyName string ). Another workaround is to modify function that add DLLs to TPA list to not add our assembly - I've just used function from coreCLR code which adds all files.


## Example
### Building example
  Run make:

    make
  Then *Managed.dll* and *SCCH* will appear in your directory.

### Running example
  Just run ./SCCH with proper arguments:
    ./SCCH "$(dirname $(which dnx))" ./Managed.dll Managed runIt
