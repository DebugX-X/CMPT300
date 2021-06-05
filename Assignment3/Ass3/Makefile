all: Lets-talk.c list.c server.c client.c
	gcc -g -Wall -o lets-talk Lets-talk.c list.c server.c client.c -lpthread
valgrind:
	#hardcoding parameters for valgrind @250
	valgrind --leak-check=full ./lets-talk 3000 localhost 3001
clean:
	$(RM) Lets-talk