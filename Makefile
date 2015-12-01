ifndef SCCH_COREPATH
$(error SCCH_COREPATH is not set!)
endif

CPP = g++53
WARN_LEVEL = -Wall -Wextra -Werror
C_STD = -std=c++11
MCS = mcs
LIBS = -ldl -lstdc++fs

.PHONY: all clean

all: SCCH Managed.dll Makefile

SCCH: simpleCoreCLRHost.cpp simpleCoreCLRHost.hpp utils.hpp Makefile
	$(CPP) $(WARN_LEVEL) $(C_STD) simpleCoreCLRHost.cpp -o SCCH $(LIBS)

Managed.dll: Managed.cs Makefile
	$(MCS) -noconfig -nostdlib -r:"$(SCCH_COREPATH)/mscorlib.dll" -r:"$(SCCH_COREPATH)/System.Runtime.dll" -r:"$(SCCH_COREPATH)/System.Console.dll" -t:library -unsafe -out:Managed.dll Managed.cs
clean:
	rm -rf SCCH SCCH.dSYM Managed.dll
