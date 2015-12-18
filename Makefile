ifndef SCCH_COREPATH
  $(error SCCH_COREPATH is not set!)
endif

OS_NAME = $(shell uname -s)
ifeq ($(OS_NAME), Darwin)
  CXX = g++-5
  ifeq (, $(shell which $(CPP)))
    $(error "$(CPP) not found! You need to install gcc (5.3) via Homebrew to build this!")
	endif
else
  CXX = g++
endif

CXXFLAGS = -Wall -Wextra -Werror -std=c++14
LDLIBS = -ldl -lstdc++fs
CSHARP = mcs
CSHARPFLAGS = -noconfig -nostdlib -unsafe
CSHARPLIBS = -r:"$(SCCH_COREPATH)/mscorlib.dll" -r:"$(SCCH_COREPATH)/System.Runtime.dll" -r:"$(SCCH_COREPATH)/System.Console.dll"

.PHONY: all clean

all: SCCH Managed.dll Makefile

SCCH: simpleCoreCLRHost.cpp simpleCoreCLRHost.hpp utils.hpp Makefile dynamicLinker.cpp dynamicLinker.hpp
	$(CXX) $(CXXFLAGS) simpleCoreCLRHost.cpp dynamicLinker.cpp -o SCCH $(LDLIBS)

Managed.dll: Managed.cs Makefile
	$(CSHARP) $(CSHARPFLAGS) -t:library -out:Managed.dll Managed.cs $(CSHARPLIBS)
clean:
	rm -rf SCCH SCCH.dSYM Managed.dll
