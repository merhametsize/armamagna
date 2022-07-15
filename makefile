CFLAGS=-c -O2 -std=c++11
LFLAGS=-s -lpthread
OBJFILES = obj/main.o obj/WordSignature.o obj/SmartDictionary.o obj/StringNormalizer.o obj/PowerSet.o obj/CommandLineParser.o obj/SearchThread.o obj/ArmaMagna.o

target: $(OBJFILES)
	mv *.o obj
	g++ $(LFLAGS) -o armamagna $(OBJFILES)

obj/main.o: src/main.cpp
	g++ $(CFLAGS) src/main.cpp

obj/WordSignature.o: src/WordSignature.cpp src/WordSignature.h
	g++ $(CFLAGS) src/WordSignature.cpp

obj/SmartDictionary.o: src/SmartDictionary.cpp src/SmartDictionary.h
	g++ $(CFLAGS) src/SmartDictionary.cpp

obj/StringNormalizer.o: src/StringNormalizer.cpp src/StringNormalizer.h
	g++ $(CFLAGS) src/StringNormalizer.cpp

obj/ArmaMagna.o: src/ArmaMagna.cpp src/ArmaMagna.h
	g++ $(CFLAGS) src/ArmaMagna.cpp

obj/PowerSet.o: src/PowerSet.cpp src/PowerSet.h
	g++ $(CFLAGS) src/PowerSet.cpp

obj/CommandLineParser.o: src/CommandLineParser.cpp src/CommandLineParser.h
	g++ $(CFLAGS) src/CommandLineParser.cpp

obj/SearchThread.o: src/SearchThread.cpp src/SearchThread.h
	g++ $(CFLAGS) src/SearchThread.cpp

clean:
	rm -f obj/*.o
