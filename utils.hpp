//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//

#include <cstdlib>
#include <set>
#include <string>
#include <cstring>
#include <sys/stat.h>
#include <dirent.h>

// Prototype of the coreclr_initialize function from the libcoreclr.so
typedef int (*coreclrInitializeFunction)(
            const char* exePath,
            const char* appDomainFriendlyName,
            int propertyCount,
            const char** propertyKeys,
            const char** propertyValues,
            void** hostHandle,
            unsigned int* domainId);

// Prototype of the coreclr_shutdown function from the libcoreclr.so
typedef int (*coreclrShutdownFunction)(
            void* hostHandle,
            unsigned int domainId);

// Prototype of the coreclr_execute_assembly function from the libcoreclr.so
typedef int (*coreclrCreateDelegateFunction)(
              void* hostHandle,
              unsigned int domainId,
              const char* entryPointAssemblyName,
              const char* entryPointTypeName,
              const char* entryPointMethodName,
              void** delegate);


void AddFilesFromDirectoryToTpaList( std::string directory, std::string& tpaList ) {

  DIR* dir = opendir(directory.c_str());
  if ( dir == nullptr ) {
    std::cerr << "ERROR: coreCLR dir is empty!" << std::endl;
    return;
  }

  std::set<std::string> addedAssemblies;

  const char* ext = ".dll";
  int extLength = strlen(ext);

  struct dirent* entry;

  // For all entries in the directory
  while ( (entry = readdir(dir)) != nullptr ) {
    if ( entry->d_type == DT_LNK || entry->d_type == DT_UNKNOWN ) {
      std::string fullFilename;

      fullFilename.append(directory);
      fullFilename.append("/");
      fullFilename.append(entry->d_name);

      struct stat sb;
      if ( stat(fullFilename.c_str(), &sb) == -1 )
        continue;

      if ( !S_ISREG(sb.st_mode) )
        continue;
    } else if ( entry->d_type != DT_REG ) {
      continue;
    }

    std::string filename(entry->d_name);

    // Check if the extension matches the one we are looking for
    int extPos = filename.length() - extLength;
    if ( (extPos <= 0) || (filename.compare(extPos, extLength, ext) != 0) )
      continue;

    std::string filenameWithoutExt( filename.substr(0, extPos) );

    // Make sure if we have an assembly with multiple extensions present,
    // we insert only one version of it.
    if ( addedAssemblies.find(filenameWithoutExt) == addedAssemblies.end() ) {
      addedAssemblies.insert(filenameWithoutExt);

      tpaList.append(directory).append("/").append(filename).append(":");
    }
  }

  closedir(dir);
}
