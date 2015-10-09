# simpleCoreCLRHost

  This c++ app allows to run custom C# method from compiled C# .dll on Linux and OS X using coreCLR.
  It's code is based on [example from coreCLR](https://github.com/dotnet/coreclr/tree/master/src/coreclr/hosts/unixcoreruncommon)
  and was done with big help from [**@janvorli**](https://github.com/janvorli), who patiently answered my questions.

## Prerequisites
  You need DNX SDK
  ( [Linux](https://github.com/dotnet/coreclr/blob/master/Documentation/install/get-dotnetcore-dnx-linux.md), [OS X](https://github.com/dotnet/coreclr/blob/master/Documentation/install/get-dotnetcore-dnx-osx.md) )
  and compiler with c++11 support. You will also need *Mono* to build example.

## Compilation
  Just link with dl, and use c++11 standard.

    clang++ -ldl -std=c++11 simpleCoreCLRHost.cpp -o SCCH
  or

    g++ -ldl -std=c++11 simpleCoreCLRHost.cpp -o SCCH
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
  First, set $SCCH_COREPATH variable in your shell to absolute path of **YOUR** directory with DNX dlls.
  Mine is:

    export SCCH_COREPATH="$HOME/.dnx/runtimes/dnx-coreclr-darwin-x64.1.0.0-rc1-15838/bin"
  then, go to ./example dir and run build.sh script:

    cd ./example
    sh build.sh
  and *Hello.dll* will appear in your *./example* dir.

### Running example
  Just go back, and run ./SCCH:

    cd ..
    ./SCCH "$SCCH_COREPATH" ./example/Hello.dll Hello HelloWorld
