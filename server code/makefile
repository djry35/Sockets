ALL: chatServer
	@echo "\n\nCompilation complete. To run:\n"
	@echo "[sudo] chatServer <portNumber>\n"
	@echo "\nMake sure you are root! Otherwise it won't work.\n"

chatServer: sockets.o comms.o
	gcc -g -Werror sockets.o comms.o -o chatServer
sockets.o: sockets.c sockets.h
	gcc -c -g -Werror sockets.c
comms.o: comms.c sockets.h
	gcc -c -g -Werror comms.c
clean:
	rm *.o chatServer
