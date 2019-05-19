server:
	gcc -o server server.c -lrt
client:
	gcc -o client client.c -lrt


all: server client

clean:
	rm client server

rebuild: clean all
