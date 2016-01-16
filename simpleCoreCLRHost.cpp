/*
 *  Copyright (c) Hubert Jarosz. All rights reserved.
 *  Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

#include "simpleCoreCLRHost.hpp"
#include "utils.hpp"

int runFromEntryPoint(
            std::string currentExeAbsolutePath,
            std::string clrFilesAbsolutePath,
            std::string managedAssemblyAbsoluteDir,
            std::string assemblyName,
            std::string entryPointType,
            std::string entryPointName)
{

  std::string coreClrDllPath = clrFilesAbsolutePath + "/" + coreClrDll;


  if ( coreClrDllPath.size() >= PATH_MAX ) {
      std::cerr << "Path to libcoreclr.so too long!" << std::endl;
      return -1;
  }

  std::string nativeDllSearchDirs = managedAssemblyAbsoluteDir + ":" + clrFilesAbsolutePath;

  std::string tpaList;
  AddFilesFromDirectoryToTpaList( clrFilesAbsolutePath, tpaList );

  auto dl = dynamicLinker::dynamicLinker::make_new( coreClrDllPath );
  auto coreclr_initialize = dl->getFunction<coreclrInitializeFunction>("coreclr_initialize");
  auto coreclr_shutdown = dl->getFunction<coreclrShutdownFunction>("coreclr_shutdown");
  auto coreclr_create_delegate = dl->getFunction<coreclrCreateDelegateFunction>("coreclr_create_delegate");

  try {
    dl->open();
    coreclr_initialize.init();
    coreclr_shutdown.init();
    coreclr_create_delegate.init();
  } catch ( dynamicLinker::openException e ) {
    std::cerr << "Cannot find " << coreClrDll << "Path that was searched: "
              << coreClrDllPath << std::endl;
    std::cerr << e.what() << std::endl;
    return -1;
  } catch ( dynamicLinker::symbolException e ) {
    std::cerr << "Probably your libcoreclr is broken or too old." << std::endl;
    std::cerr << e.what() << std::endl;
    return -1;
  } catch ( dynamicLinker::dynamicLinkerException e ) {
    std::cerr << e.what() << std::endl;
    return -1;
  }

  const char *propertyKeys[] = {
      "TRUSTED_PLATFORM_ASSEMBLIES",
      "APP_PATHS",
      "APP_NI_PATHS",
      "NATIVE_DLL_SEARCH_DIRECTORIES",
      "AppDomainCompatSwitch"
  };

  const char *propertyValues[] = {
      tpaList.c_str(),
      managedAssemblyAbsoluteDir.c_str(),
      managedAssemblyAbsoluteDir.c_str(),
      nativeDllSearchDirs.c_str(),
      "UseLatestBehaviorWhenTFMNotSpecified"
  };

  void* hostHandle = NULL;
  unsigned int domainId = 0;
  int status = -1;

  // initialize coreclr
  try {
    status = coreclr_initialize (
      currentExeAbsolutePath.c_str(),
      "simpleCoreCLRHost",
      sizeof(propertyKeys) / sizeof(propertyKeys[0]),
      propertyKeys,
      propertyValues,
      &hostHandle,
      &domainId
    );
  } catch ( dynamicLinker::dynamicLinkerException e ) {
    std::cerr << e.what() << std::endl;
    return -1;
  }

  if ( status < 0 ) {
    std::cerr << "ERROR! coreclr_initialize status: 0x" << std::hex << status << std::endl;
    return -1;
  }

  // Fancy modern C++ code. You can also just use void *.
  auto del = []( __attribute__((unused)) csharp_runIt_t * ptr ) {};
  auto csharp_runIt = std::unique_ptr<csharp_runIt_t, decltype(del)>(nullptr, del);

  try {
    // create delegate to our entry point
    status = coreclr_create_delegate (
      hostHandle,
      domainId,
      assemblyName.c_str(),
      entryPointType.c_str(),
      entryPointName.c_str(),
      reinterpret_cast<void**>(&csharp_runIt)
    );
  } catch ( dynamicLinker::dynamicLinkerException e ) {
    std::cerr << e.what() << std::endl;
    return -1;
  }

  if ( status < 0 ) {
    std::cerr << "ERROR! coreclr_create_delegate status: 0x" << std::hex << status << std::endl;
    return -1;
  }

  myClass tmp = myClass();
  tmp.question();

  /*
   *  If arguments are in in different order then second arg is 0 in C#. Dunno why.
   */
  (*csharp_runIt)( tmp, std::mem_fun_ref(&myClass::print) );

  try {
    status = coreclr_shutdown ( hostHandle, domainId );
  } catch ( dynamicLinker::dynamicLinkerException e ) {
    std::cerr << e.what() << std::endl;
    return -1;
  }

  if ( status < 0 ) {
    std::cerr << "ERROR! coreclr_shutdown status: 0x" << std::hex << status << std::endl;
    return -1;
  }

  return 0;
}

int main( int argc, char* argv[] ) {

  if ( argc != 5 ) {
    std::cout << "READ README.md !" << std::endl;
    return 0;
  }

  std::string cwd = SCCH_fs::current_path();
  cwd += "/";

  std::string assemblyName(argv[2]);
  std::string assemblyDir(assemblyName);


  if( !assemblyName.size() ) {
    std::cerr << "ERROR: Bad ASSEMBLY_PATH !" << std::endl;
    return 0;
  }

  size_t find = assemblyName.rfind('/');
  if( find == std::string::npos )
    find = 0;

  assemblyName = assemblyName.substr( find+1, assemblyName.size() );

  if( assemblyName.size() < 5 ||
      assemblyName.substr( assemblyName.size()-4,
                           assemblyName.size()) != ".dll" ) {
    std::cerr << "ERROR: Assembly is not .dll !" << std::endl;
    return 0;
  }

  assemblyName = assemblyName.substr( 0, assemblyName.size()-4 );

  assemblyDir.erase(find);  // get dir of assembly
  assemblyDir = cwd + assemblyDir;


  int exitCode = runFromEntryPoint(
                          cwd+std::string(argv[0]), // absolute path to this exe
                          std::string(argv[1]),     // absolute path to coreCLR DLLs
                          assemblyDir, // absolute path to DLL to run
                          assemblyName,
                          std::string(argv[3]),
                          std::string(argv[4]));

  if ( exitCode < 0 )
    std::cout << "Exit Code: " << exitCode << std::endl;


  return 0;
}
