ALL: chatBot
	@echo "\nCompilation complete. To run:"
	@echo "\n[sudo] chatBot <server address> <port number>"
	@echo "\nMake sure you are root! It will not work otherwise.\n"
chatBot: comms.o sockets.o parser.o
	gcc -g -Werror comms.o sockets.o parser.o -o chatBot
comms.o: comms.c sockets.h
	gcc -c -g -Werror comms.c
sockets.o: sockets.c sockets.h
	gcc -c -g -Werror sockets.c
parser.o: parser.c sockets.h
	gcc -c -g -Werror parser.c
clean:
	rm *.o chatBot
