#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<wait.h>
#define command_sep " \t\n\r\a"
char ** separguements(char * cmd)                                                         //Used to tokenise the input commands
{
	int i=0;
	char ** buf = malloc(50*sizeof(char*));
	if(buf == NULL)
	{
		printf("Fail to allocate memory in separguements\n");
		exit(EXIT_FAILURE);
	}
		char * token = strtok(cmd,command_sep);
		while (token != NULL) 
	{
		buf[i] = token;
		i++;
		if(i >= sizeof(buf)/sizeof(char*))
		{
			buf = realloc(buf,50*sizeof(char*));
			if(buf == NULL)
			{
				printf("Fail to reallocate in separguements\n");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL,command_sep);
	}
	buf[i]=NULL;
	return buf;
}
int main(int argc,char const *argv[])
{
    while(1)
    {
    //struct sockaddr_in local_address;
    struct sockaddr_in server_address;                                          
    /*socket() creates an endpoint for communication and returns a descriptor.
    domain :- The  domain argument specifies a communication domain; 
        this selects the protocol family which will be used for communication.
    type:- SOCK_STREAM Provides sequenced, reliable, two-way, connection-based byte streams.
        Sockets  of  type  SOCK_STREAM  are full-duplex byte streams.  
        They do not preserve record boundaries.  A stream socket must be in a
       connected state before any data may be sent or received on it. 
    protocol :- The  protocol  specifies a particular protocol to be used with the socket.*/
    int socket_init = socket(AF_INET,SOCK_STREAM,0);
    if( socket_init < 0)
    {
        printf("Error  in socket creation of client\n");
        return -1;
    }
    explicit_bzero(&server_address,sizeof(server_address));       //intialising memory to null bytes
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    /*The htons() function converts the unsigned short integer hostshort 
        from host byte order to network byte order.*/
    if(inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr)<=0)
    {
			printf("The entered ip address is invalid or wrong - inet_pton ERROR\n");
			return -1;
	}
    /* Converts an IP address in numbers-and-dots notation into either a 
		// struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.*/
    int connection = connect(socket_init,(struct sockaddr *)&server_address,sizeof(server_address));
    if(connection < 0)
    {
        printf("Connection Failed\n");
        return -1;
    }
    /*
		   int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
		   The connect() system call connects the socket referred to by the file descriptor sockfd to the 
           address specified by addr. Server’s address and port is specified in addr.*/
    else
    {
        printf("Connection Established with server\n");
        //while(1)
        {
            printf("Enter the command\n");
            char *takecommand;
            size_t cmdlength = 0;
            getline(&takecommand,&cmdlength,stdin);
            char x[1000];
            strcpy(x,takecommand);
            int m = strlen(x);
            x[m] = '\0';
            char ** args = separguements(x);
            int j = 0;
            int size = 0;
            while(args[size]!= NULL)
            {
                size++;
            }
            if(strcmp(args[0],"send") == 0)
            {
                if(size == 1)
                {
                    printf("Please Enter the file name\n");
                    continue;
                }
                else if(size > 2)
                {
                    printf("Please Enter one file only\n");
                    continue;
                }
                int i =1;
                int filedata = 0;
			    send(socket_init , args[0] , sizeof(args[0]) , 0 );
                /*ssize_t send(int sockfd, const void *buf, size_t len, int flags);
            the message is found in buf and has length len. */
                char ack[3];
                recv(socket_init,ack,3,0);
                    FILE *newfile;
                    int check = 0;
			        send(socket_init , args[i] , sizeof(args[i]) , 0 );
                    char mybuffer[300],test[10];
                    explicit_bzero(&test,sizeof(test));
                    read(socket_init,test,sizeof(test));
                    int m = strlen(test);
                    test[m] = '\0';
                    if(strcmp(test,"error") == 0)
                    {
                        printf("No such file\n");
                        continue;
                    }                    
                    newfile = fopen(args[i], "w");
                    if(newfile == NULL)
                    {
                        printf("Error opening file\n");
                        exit(-1);
                    }
                    explicit_bzero(&mybuffer,sizeof(mybuffer));
                    while((filedata = read(socket_init,mybuffer, 256)) > 0)
                    {
                        if((fwrite(mybuffer,sizeof(char),filedata,newfile)) < filedata)
                        {
                            perror("File write failed! ");
						    exit(-1);
                        }
                        explicit_bzero(&mybuffer,sizeof(mybuffer)); 
                        if(filedata!=256)
                        {
                            break;
                        }
                    }
                    if(filedata < 0)
                    {
                        printf("Reading Error\n");
                    }
                    else
                    {
                        printf("File downloaded successfully\n");
                        fclose(newfile);  
                    }
                    continue;
            }
            else if(strcmp(args[0],"quit") == 0)
            {
                int tru = 1;
                close(socket_init);
                setsockopt(socket_init,SOL_SOCKET,SO_REUSEADDR,&tru,sizeof(int));
                exit(-1);
            }
            else if(strcmp(args[0],"listall") == 0)
            {
                char my[1000];
                explicit_bzero(&my,sizeof(my));
			    send(socket_init , args[0] , sizeof(args[0]) , 0 );
                recv(socket_init,my,sizeof(my),0);
                printf("%s",my);
            }
            else
            {
                printf("Invalid Command\n");
            }
        }
    }
    close(socket_init);
}
    return 0;
}
