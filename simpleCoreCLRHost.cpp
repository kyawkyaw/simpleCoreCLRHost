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
        ExecuteAssemblyFunction executeAssembly = (ExecuteAssemblyFunction)dlsym( coreclrLib, "ExecuteAssembly" );
        if ( executeAssembly != nullptr ) {
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

            HRESULT st = executeAssembly(
                            currentExeAbsolutePath.c_str(),
                            coreClrDllPath.c_str(),
                            "simpleCoreCLRHost",
                            sizeof(propertyKeys) / sizeof(propertyKeys[0]),
                            propertyKeys,
                            propertyValues,
                            0,    // argc
                            NULL, // argv
                            NULL,
                            assemblyName.c_str(),
                            entryPointType.c_str(),
                            entryPointName.c_str(),
                            (DWORD*)&exitCode);

            if ( !SUCCEEDED( st ) ) {
                std::cerr << "ExecuteAssembly failed - status: " << st << std::endl;
                exitCode = -1;
            }
        }
        else
            std::cerr << "ERROR: Function ExecuteAssembly not found in the libcoreclr.so" << std::endl;

        if ( dlclose( coreclrLib ) != 0 )
            std::cerr << "WARNING: dlclose failed" << std::endl;

    }
    else
        std::cerr << "ERROR: dlopen failed to open the libcoreclr.so with error " << dlerror() << std::endl;

    return exitCode;
}

int main( int argc, char* argv[]) {

  if ( argc != 5 ) {
    std::cout << "READ README.md !" << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "./a.out DLL_PATH ASSEMBLY_PATH ENTRY_TYPE ENTRY_NAME" << std::endl;
    std::cout << "ALL PATHs must be relative! ASSEMBLY cannot be in the same DIR as DLLS!" << std::endl;
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
                          cwd+std::string(argv[0]),
                          cwd+std::string(argv[1]),
                          cwd+std::string(argv[2]),
                          assemblyName, // "Hello, Version=0.0.0.0"
                          std::string(argv[3]), // "Hello"
                          std::string(argv[4])); // "HelloWorld"
  std::cout << "Exit Code: " << exitCode << std::endl;
  return 0;
}
