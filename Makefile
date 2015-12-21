#
#  Copyright (c) Hubert Jarosz. All rights reserved.
#  Licensed under the MIT license. See LICENSE file in the project root for full license information.
#

OS_NAME = $(shell uname -s)
ifeq ($(OS_NAME), Darwin)
  CXX = g++-5
  ifeq (, $(shell which $(CPP)))
    $(error "$(CPP) not found! You need to install gcc (5.3) via Homebrew to build this!")
	endif
else
  CXX = g++
endif

SCCH_COREPATH = $(shell dirname $(shell which dnx))
CXXFLAGS = -Wall -Wextra -Werror -std=c++11
LDLIBS = -ldl -lstdc++fs
CSHARP = mcs
CSHARPFLAGS = -noconfig -nostdlib -unsafe
CSHARPLIBS = -r:"$(SCCH_COREPATH)/mscorlib.dll" -r:"$(SCCH_COREPATH)/System.Runtime.dll" -r:"$(SCCH_COREPATH)/System.Console.dll"

.PHONY: all clean

all: SCCH Managed.dll Makefile

SCCH: simpleCoreCLRHost.cpp simpleCoreCLRHost.hpp utils.hpp Makefile
	git -C dynamicLinker pull || git clone https://github.com/Marqin/dynamicLinker
	make -C dynamicLinker CXX=$(CXX)
	$(CXX) $(CXXFLAGS) simpleCoreCLRHost.cpp dynamicLinker/dynamicLinker.o -o SCCH $(LDLIBS)

Managed.dll: Managed.cs Makefile
	$(CSHARP) $(CSHARPFLAGS) -t:library -out:Managed.dll Managed.cs $(CSHARPLIBS)

clean:
	rm -rf SCCH SCCH.dSYM Managed.dll
	sh -c "stat dynamicLinker/ &> /dev/null && make -C dynamicLinker clean" || true

distclean: clean
	rm -rf dynamicLinker/
