CFLAGS=-c -O2 -std=c++11
LFLAGS=-s -lpthread
OBJFILES = main.o WordSignature.o SmartDictionary.o StringNormalizer.o PowerSet.o CommandLineParser.o SearchThread.o ArmaMagna.o

target: $(OBJFILES)
	g++ $(LFLAGS) -o armamagna $(OBJFILES)

main.o: src/main.cpp
	g++ $(CFLAGS) src/main.cpp

WordSignature.o: src/WordSignature.cpp src/WordSignature.h
	g++ $(CFLAGS) src/WordSignature.cpp

SmartDictionary.o: src/SmartDictionary.cpp src/SmartDictionary.h
	g++ $(CFLAGS) src/SmartDictionary.cpp

StringNormalizer.o: src/StringNormalizer.cpp src/StringNormalizer.h
	g++ $(CFLAGS) src/StringNormalizer.cpp

ArmaMagna.o: src/ArmaMagna.cpp src/ArmaMagna.h
	g++ $(CFLAGS) src/ArmaMagna.cpp

PowerSet.o: src/PowerSet.cpp src/PowerSet.h
	g++ $(CFLAGS) src/PowerSet.cpp

CommandLineParser.o: src/CommandLineParser.cpp src/CommandLineParser.h
	g++ $(CFLAGS) src/CommandLineParser.cpp

SearchThread.o: src/SearchThread.cpp src/SearchThread.h
	g++ $(CFLAGS) src/SearchThread.cpp

clean:
	rm -f *.o
