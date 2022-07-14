CFLAGS=-Wall -c -O2 -std=c++11
LFLAGS=-Wall -s -lpthread

target: main.o WordSignature.o SmartDictionary.o StringNormalizer.o PowerSet.o CommandLineParser.o SearchThread.o ArmaMagna.o
	g++ $(LFLAGS) -o armamagna main.o WordSignature.o SmartDictionary.o StringNormalizer.o PowerSet.o CommandLineParser.o SearchThread.o ArmaMagna.o

main.o: main.cpp
	g++ $(CFLAGS) main.cpp

WordSignature.o: WordSignature.cpp WordSignature.h
	g++ $(CFLAGS) WordSignature.cpp

SmartDictionary.o: SmartDictionary.cpp SmartDictionary.h
	g++ $(CFLAGS) SmartDictionary.cpp

StringNormalizer.o: StringNormalizer.cpp StringNormalizer.h
	g++ $(CFLAGS) StringNormalizer.cpp

ArmaMagna.o: ArmaMagna.cpp ArmaMagna.h
	g++ $(CFLAGS) ArmaMagna.cpp

PowerSet.o: PowerSet.cpp PowerSet.h
	g++ $(CFLAGS) PowerSet.cpp

CommandLineParser.o: CommandLineParser.cpp CommandLineParser.h
	g++ $(CFLAGS) CommandLineParser.cpp

SearchThread.o: SearchThread.cpp SearchThread.h
	g++ $(CFLAGS) SearchThread.cpp

clean:
	rm -f *.o
