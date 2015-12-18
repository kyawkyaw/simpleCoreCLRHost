/*
 *  Copyright (c) Hubert Jarosz. All rights reserved.
 *  Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

#if not defined (__unix__) && not defined(__unix) && not defined (unix) && ( (not defined (__APPLE__) || not defined (__MACH__)) )
  #error THIS SOFTWARE IS ONLY FOR UNIX-LIKE SYSTEMS!
#endif

#pragma once

#include <memory>
#include <iostream>
#include <dlfcn.h>
#include <functional>

namespace dynamicLinker {

  class _void {
  private:
    void * myself = nullptr;
  public:
    _void( void * ptr ) : myself(ptr) {}
    ~_void() {
      if( myself != nullptr )
        free(myself);
    }
    void * ptr() const {
      return myself;
    }
    void null() {
      myself = nullptr;
    }
  };

  class dynamicLinker {
  private:
    std::string libPath = "";
    std::unique_ptr<_void> lib = nullptr;
  public:
    dynamicLinker( std::string );
    ~dynamicLinker();
    bool open();
    bool explicitClose();
    template<typename T> std::function<T> getFunction( std::string name ) {
      return std::function<T>(reinterpret_cast< T * >( dlsym( lib->ptr(), name.c_str() ) ));
    }
  };
};
