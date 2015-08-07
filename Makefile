echoserver: main.o
	gcc -o echoserver main.o -Wall -l tls

main.o: main.c
	gcc -o main.o main.c -c -pedantic -Wall







