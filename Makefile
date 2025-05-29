server:Tcpserver.cc
	g++ -o $@ $^ -std=c++17 -I/home/ubuntu/personal-web/include -L/home/ubuntu/personal-web/lib -ljsoncpp -lpthread -lmd4c-html -lmd4c
.PHONY:clean
clean:
	rm -rf server http.log