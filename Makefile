LIBS=-pthread
OBJECT=server client
all : $(OBJECT)

server : main.cpp server.cpp strtool.cpp
	g++ -std=c++11 $^ -o $@ $(LIBS)

client : client.cpp strtool.cpp
	g++ -std=c++11 $^ -o $@ $(LIBS)

clean :
	rm -f *.o $(OBJECT)
