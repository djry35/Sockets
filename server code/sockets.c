#include "sockets.h"

int main(int argc, char** argv)
{
	while(1)
	{
		//gets a socket to use
		int SD = createSocket(argv[1]);
		if(SD < 0)
		{
			return 1;
		}
		//utility
		strcpy(portNum, argv[1]);
		//the socket can now listen through the port it was assigned to.
		//the 20 declares the max number of connections that could be queued in that port
		if(listen(SD, 20) == -1)
		{
			perror("listen error");
			return 1;
		}

		printf("Socket established, awaiting client...\n");

		int newSD;
		struct addrinfo peerInfo;
		memset((void*)&peerInfo, 0, sizeof(struct addrinfo));	
		//everything is set up, now we wait for a client to connect to the server.
		//There's a way to not have this be an infinite loop (accept() can hang until a connection), 
		//but I didn't find out, since this works just as well.
		while((newSD = accept(SD, peerInfo.ai_addr, &(peerInfo.ai_addrlen))) == -1
			&& (errno == EAGAIN || errno == EWOULDBLOCK));

		if(newSD == -1)
		{
			perror("accept error");
			return 1;
		} 
		else
		{
			printf("connection established\n");
		}
	
		//so now that we have a client connected, prepare the functions to be called to proces their message.
		//We know that it can only be one of four messages, and errors based on that is client-side.
		int (*cmdProc[4])(msg_t, msg_t*);
		cmdProc[LOGIN] = login;
		cmdProc[NEWUSR] = newUser;
		cmdProc[SEND] = sendMsg;
		cmdProc[LOGOUT] = logout;
		
		bool errflag = false;

		while(1)
		{
			msg_t m;
			//attempt to read the message from the client.
			//Again, read() might hang by itself, but loop it just in case.
			//Too awkward for this to actually fail, so will ignore for now.
			while(read(newSD, (void*)&m, sizeof(msg_t)) != sizeof(msg_t));
			printf("message received.\n");
			msg_t retMsg;
			//process the message received, and prep the message to be sent back.
			if(cmdProc[m.type](m, &retMsg) < 0)
			{
				//not sure what to do here, honestly. Client receiving an error has not been implemented.
				printf("Fatal error?\n");
				errflag = true;
			}
		
			//attempt to send the message to the client.
			//Probably hangs like above, not sure. If this fails, what happens?
			while(write(newSD, (void*)&retMsg, sizeof(msg_t)) != sizeof(msg_t));
			printf("message sent.\n");

			if(errflag == true && (m.type == LOGIN || m.type == NEWUSR))
			{
				printf("Connection ended.\n");
				close(newSD);
				while((newSD = accept(SD, peerInfo.ai_addr, &(peerInfo.ai_addrlen))) == -1
			&& (errno == EAGAIN || errno == EWOULDBLOCK));
				errflag = false;
				printf("connection established\n");
			}
			//if the client is leaving, we'll disconnect
			if(m.type == LOGOUT)
			{
				printf("Connection ended.\n");
				close(newSD);
				close(SD);
				break;
			}		
		}
		//This gives ample time for the OS to clean out the socket we were using, so that we can get another one prepped.
		//Not sure what the shortest time could be.
		sleep(60);
		printf("Awoken, testing new socket...\n");
	}	
	//SHOULD NEVER GET HERE	
	return 0;
}


int createSocket(char* portNum)
{
	//Okay, to prep the socket for the server:
	//1. set up the configuration that the server will allow
	//2. find the addresses that can be used in the socket description, returned from getaddrinfo as a linked list
	//3. test each address (maybe not needed? Is there only one address? idk)
	//4. Once we find an address to use, we bind the socket to the address:port we want.
	//Once all done, return the socket descriptor
	struct addrinfo hints;
	memset((void*)&hints, 0, sizeof(struct addrinfo));
	//declare the settings we care about for the server socket
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	struct addrinfo* res;
	//given the settings above, find an address:port we can use
	if(getaddrinfo(NULL, portNum, &hints, &res) != 0)
	{
		perror("Address Error");
		return -1;
	}
	struct addrinfo* rptr = res;
	int SD;
	while(rptr)
	{
		//for each possible address:port configuration, see if we can make a socket with configs
		if((SD = socket(rptr->ai_family, rptr->ai_socktype, 
			rptr->ai_protocol)) >= 0)
		{
			//made a socket, so bind the socket to that address:port
			if(bind(SD, rptr->ai_addr, rptr->ai_addrlen) >= 0)
			{
				//success!
				break;
			}
			else
			{
				perror("bind error");
				close(SD);
			}
		}
		
		rptr = rptr->ai_next;
	}
	//if rptr got to the end of the list, then none of them worked
	if(!rptr)
	{
		printf("No sockets created. Are you root?\n");
		return -1;
	}
	
	//sets up what the OS will do with the socket when we close it. 
	//Makes the socket reusable for reconfiguring. Not sure why it doesn't work without it...?
	bool b = true;
	setsockopt(SD, SOL_SOCKET, SO_REUSEADDR, &b, sizeof(int));
	
	return SD;
}

