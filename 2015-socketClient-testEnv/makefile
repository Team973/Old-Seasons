CPP=g++
CPPFLAGS=-std=gnu++11 -pthread

servertest: servertest.o socketClient.o
	$(CPP) $(CPPFLAGS) servertest.o socketClient.o -o servertest

servertest.o: servertest.cpp
	$(CPP) $(CPPFLAGS) -c servertest.cpp

socketClient.o: socketClient.cpp socketClient.hpp
	$(CPP) $(CPPFLAGS) -c socketClient.cpp socketClient.hpp 

clean:
	rm *.o servertest
