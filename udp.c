#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void UDPSend(char *message, int port)
{ 
	if (port > 0)
	{
		int sd, rc;
		struct sockaddr_in cliAddr, remoteServAddr;
		struct hostent *h;
		int broadcast = 1;
	  
		h = gethostbyname("255.255.255.255");
		remoteServAddr.sin_family = h->h_addrtype;
		memcpy((char *) &remoteServAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
		remoteServAddr.sin_port = htons(port);

		// socket creation
		sd = socket(AF_INET,SOCK_DGRAM,0);
		if (sd<0)
		{
			printf("Cannot open socket on port %d\n", port);
			return;
		}
		
		if (setsockopt(sd, SOL_SOCKET, SO_BROADCAST, &broadcast,sizeof broadcast) == -1)
		{
			printf("setsockopt (SO_BROADCAST)");
			return;
		}
	  
		// bind any port
		cliAddr.sin_family = AF_INET;
		cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		cliAddr.sin_port = htons(0);
	  
		rc = bind(sd, (struct sockaddr *) &cliAddr, sizeof(cliAddr));
		if (rc<0)
		{
			printf("Cannot bind port %d\n", port);
			return;
		}

		// send data
		rc = sendto(sd, message, strlen(message)+1, 0, (struct sockaddr *) &remoteServAddr, sizeof(remoteServAddr));

		if (rc<0)
		{
			printf("%s: cannot send data %s to port %d \n", message, port);
			close(sd);
			return;
		}

		printf("Send %d bytes to port %d\n", rc, port);
		
		close(sd);
	}
}

void main(int argc, char *argv[])
{
	int Port;
	char *Message;

	Port = 0;
	if (argc > 2)
	{
		Port = atoi(argv[1]);
		Message = argv[2];
	}

	if (Port > 0)
	{
		UDPSend(Message, Port);
	}
	else
	{
		printf("Usage: udp <port> <message>\n");
	}
}

