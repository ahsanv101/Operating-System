server:
	gcc -o server client.c -lrt
client:
	gcc -o client server.c -lrt


all: server client

clean:
	rm client server

rebuild: clean all
