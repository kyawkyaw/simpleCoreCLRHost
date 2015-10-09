//
// parts of runFromEntryPoint - Copyright (c) Microsoft. All rights reserved.
// rest - Copyright (c) Hubert Jarosz. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//

#include "utils.hpp"

#include <unistd.h>

int runFromEntryPoint(
            std::string currentExeAbsolutePath,
            std::string clrFilesAbsolutePath,
            std::string managedAssemblyAbsolutePath,
            std::string assemblyName,
            std::string entryPointType,
            std::string entryPointName)
{
    int exitCode = -1;

    std::string coreClrDllPath = clrFilesAbsolutePath + "/" + coreClrDll;


    if ( coreClrDllPath.size() >= PATH_MAX ) {
        std::cerr << "Path to libcoreclr.so too long!" << std::endl;
        return -1;
    }

    // Get just the path component of the managed assembly path
    std::string appPath;
    GetDirectory( managedAssemblyAbsolutePath, appPath );

    std::string nativeDllSearchDirs = appPath + ":" + clrFilesAbsolutePath;

    std::string tpaList;
    AddFilesFromDirectoryToTpaList( clrFilesAbsolutePath, tpaList );

    void* coreclrLib = dlopen( coreClrDllPath.c_str(), RTLD_NOW | RTLD_LOCAL );

    if ( coreclrLib != nullptr ) {

        coreclrInitializeFunction coreclr_initialize = (coreclrInitializeFunction) dlsym( coreclrLib, "coreclr_initialize" );
        coreclrShutdownFunction coreclr_shutdown = (coreclrShutdownFunction) dlsym( coreclrLib, "coreclr_shutdown" );
        coreclrCreateDelegateFunction coreclr_create_delegate = (coreclrCreateDelegateFunction) dlsym( coreclrLib, "coreclr_create_delegate" );

        if ( coreclr_initialize != nullptr && coreclr_shutdown != nullptr &&
                                          coreclr_create_delegate != nullptr ) {

            const char *propertyKeys[] = {
                "TRUSTED_PLATFORM_ASSEMBLIES",
                "APP_PATHS",
                "APP_NI_PATHS",
                "NATIVE_DLL_SEARCH_DIRECTORIES",
                "AppDomainCompatSwitch"
            };
            const char *propertyValues[] = {
                tpaList.c_str(),
                appPath.c_str(),
                appPath.c_str(),
                nativeDllSearchDirs.c_str(),
                "UseLatestBehaviorWhenTFMNotSpecified"
            };

            void* hostHandle;
            unsigned int domainId;

            // initialize coreclr
            int status = coreclr_initialize (
              currentExeAbsolutePath.c_str(),
              "simpleCoreCLRHost",
              sizeof(propertyKeys) / sizeof(propertyKeys[0]),
              propertyKeys,
              propertyValues,
              &hostHandle,
              &domainId
            );

            if ( status < 0 ) {
              std::cerr << "ERROR! coreclr_initialize status: " << status << std::endl;
              return -1;
            }

            void** delegate;

            // create delegate to our entry point
            status = coreclr_create_delegate (
              hostHandle,
              domainId,
              assemblyName.c_str(),
              entryPointType.c_str(),
              entryPointName.c_str(),
              delegate
            );

            if ( status < 0 ) {
              std::cerr << "ERROR! coreclr_create_delegate status: " << status << std::endl;
              return -1;
            }

            // I have to flush cout, or coreclr will crash. Bug?
            std::cout << std::flush;

            // run our delegate
            int result = ((int (*)(int)) *delegate)(4);
            std::cout << "Result should be 8. Result is: " << result << std::endl;

            status = coreclr_shutdown ( hostHandle, domainId );

            if ( status < 0 ) {
              std::cerr << "ERROR! coreclr_shutdown status: " << status << std::endl;
              return -1;
            }
        } else
          std::cerr << "ERROR: Functions we need were not found in the libcoreclr.so" << std::endl;

        if ( dlclose( coreclrLib ) != 0 )
          std::cerr << "WARNING: dlclose failed" << std::endl;

    }
    else
      std::cerr << "ERROR: dlopen failed to open the libcoreclr.so with error " << dlerror() << std::endl;

    return 0;
}

int main( int argc, char* argv[]) {

  if ( argc != 5 ) {
    std::cout << "READ README.md !" << std::endl;
    return 0;
  }

  char _cwd[FILENAME_MAX];
  if ( !getcwd(_cwd, sizeof(_cwd)) ) {
    std::cerr << "ERROR: Cannot get CWD!" << std::endl;
    return 1;
  }

  std::string cwd(_cwd);
  cwd += "/";

  std::string assemblyName(argv[2]);

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

  int exitCode = runFromEntryPoint(
                          cwd+std::string(argv[0]), // absolute path to this exe
                          std::string(argv[1]),     // absolute path to coreCLR DLLs
                          cwd+std::string(argv[2]), // absolute path to DLL to run
                          assemblyName,
                          std::string(argv[3]),
                          std::string(argv[4]));

  if ( exitCode < 0 )
    std::cout << "Exit Code: " << exitCode << std::endl;
  return 0;
}
