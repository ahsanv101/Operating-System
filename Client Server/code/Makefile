all:
	gcc -o client client.c -pthread
	gcc -o server server.c -pthread

client:
	gcc -o client client.c -pthread

server:
	gcc -o server server.c -pthread

clean:
	rm server client

rebuild:
	clean all

