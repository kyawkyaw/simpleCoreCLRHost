ifndef SCCH_COREPATH
$(error SCCH_COREPATH is not set!)
endif

CPP = clang++
WARN_LEVEL = -Wall -Wextra -Werror
C_STD = -std=c++11
MCS = mcs

.PHONY: all clean

all: SCCH Managed.dll Makefile

SCCH: simpleCoreCLRHost.cpp simpleCoreCLRHost.hpp utils.hpp Makefile
	$(CPP) $(WARN_LEVEL) $(C_STD) -ldl simpleCoreCLRHost.cpp -o SCCH

Managed.dll: Managed.cs Makefile
	$(MCS) -noconfig -nostdlib -r:"$(SCCH_COREPATH)/mscorlib.dll" -r:"$(SCCH_COREPATH)/System.Runtime.dll" -r:"$(SCCH_COREPATH)/System.Console.dll" -t:library -unsafe -out:Managed.dll Managed.cs
clean:
	rm -rf SCCH SCCH.dSYM Managed.dll
