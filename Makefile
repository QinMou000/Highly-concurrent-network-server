server:Tcpserver.cc
	g++ -o $@ $^ -std=c++17 -ljsoncpp -lpthread #-static
.PHONY:clean
clean:
	rm -rf server http.log