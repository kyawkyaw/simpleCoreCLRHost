if [ -z "$SCCH_COREPATH" ]; then
  echo "SCCH_COREPATH not set!"
  exit 1;
fi;

cd example
mcs /noconfig /nostdlib /r:"$SCCH_COREPATH/mscorlib.dll" /r:"$SCCH_COREPATH/System.Runtime.dll" /r:"$SCCH_COREPATH/System.Console.dll" /t:library /unsafe Math.cs
clang++ --std=c++11 -c mathClass.cpp -o mathClass.o

cd ..
clang++ -ldl -std=c++11 example/mathClass.o simpleCoreCLRHost.cpp -o SCCH
