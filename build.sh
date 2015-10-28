if [ -z "$SCCH_COREPATH" ]; then
  echo "SCCH_COREPATH not set!"
  exit 1;
fi;

mcs /noconfig /nostdlib /r:"$SCCH_COREPATH/mscorlib.dll" /r:"$SCCH_COREPATH/System.Runtime.dll" /r:"$SCCH_COREPATH/System.Console.dll" /t:library /unsafe Managed.cs

clang++ -Wall -Wextra -pedantic -ldl simpleCoreCLRHost.cpp -o SCCH
