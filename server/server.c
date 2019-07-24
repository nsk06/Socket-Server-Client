#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#define command_sep " \t\n\r\a"
int selector(const struct dirent *namelist)                        //selector to ignor . & .. in listall command
{
	if(namelist->d_name[0]=='.')
		return 0;
	else
	{
		return 1;
	}
}
int main(int argc,char const *argv[])
{
    struct sockaddr_in ref_address;
    char client_ip[10];
    char rec_buffer[512];
    int tru = 1;
    int socket_init = socket(AF_INET,SOCK_STREAM,0);
    if( socket_init < 0)
    {
        printf("Error  in socket creation of server\n");
        return -1;
    }
    if (setsockopt(socket_init, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&tru, sizeof(tru)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
    explicit_bzero(&ref_address,sizeof(ref_address));
    ref_address.sin_family = AF_INET;
    ref_address.sin_port = htons(8080);
    ref_address.sin_addr.s_addr = htonl(INADDR_ANY);
    
	/*
	   int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
	   After creation of the socket, bind function binds the socket to the address and port number specified in addr(custom data structure).
	   */
    if (bind(socket_init, (struct sockaddr *)&ref_address, sizeof(ref_address))<0)
	{
		perror("Binding the socket failed\n");
		exit(-1);
	}
    printf("Server Open............\n");
       if (listen(socket_init,6) < 0) 
		{
			perror("Listening error");
			exit(-1);
		}
        /*
	   int listen(int sockfd, int backlog);
	   It puts the server socket in a passive mode, where it waits for the client to approach the server to make 
	   a connection. 
	   The backlog, defines the maximum length to which the queue of pending connections for sockfd may grow.
	   If a connection request arrives when the queue is full, the client may receive an error with an indication of ECONNREFUSED.
	   */
       
    while(1)
    {
         int connect = 0;
        int len = sizeof(ref_address);
        connect= accept(socket_init, (struct sockaddr *)&ref_address, (socklen_t*)&len);
        if (connect<0)
		{
			perror("Accept Error");
			exit(-1);
		}
        /*
		   int new_socket= accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
		   It extracts the first connection request on the queue of pending connections for the listening socket, 
		   sockfd, creates a new connected socket, and returns a new file descriptor referring to that socket.
		   At this point, connection is established between client and server, and they are ready to transfer data.
		   */
        if(inet_ntop(AF_INET, &(ref_address.sin_addr),client_ip, INET_ADDRSTRLEN)!= NULL)
		    printf("Connection with the IP established %s\n",client_ip);
        explicit_bzero(client_ip,sizeof(client_ip));
        explicit_bzero(rec_buffer,sizeof(rec_buffer));
        int received = read( connect , rec_buffer,512); 
       int pid = fork();
        if(pid == 0)
        {
            if(strcmp(rec_buffer,"quit")==0)
            {
                printf("Connection terminated with IP %s\n",client_ip);
                close(connect);
                break;
            }
            else if(strcmp(rec_buffer,"listall")==0)
            {
                char cwd[2000];
                    getcwd(cwd,sizeof(cwd));
                    strcat(cwd,"/Files/");
                    char filelist[10000];
                    explicit_bzero(&filelist,sizeof(filelist));
                    struct dirent **namelist;
                    int n = scandir(cwd, &namelist,selector, alphasort);
                    if(n < 0)
                    {
                        perror("scandir");
                    }
                    int j =0;
                while (j<n) 
                {
                    strcat(filelist,namelist[j]->d_name);
                    free(namelist[j]);
                    j++;
                    strcat(filelist,"\n");
                }
                send(connect,filelist,sizeof(filelist),0);
                continue;
            }
            else if(strcmp(rec_buffer,"send")==0)
            {
                    send(connect,"x",sizeof("x"),0);
                    char cwd[2000];
                    getcwd(cwd,sizeof(cwd));
                    strcat(cwd,"/Files/");
                    char filebuf[300];
                    explicit_bzero(filebuf,sizeof(filebuf));
                    int filename = read( connect ,filebuf,256);
                    int j = strlen(filebuf);
                    filebuf[j] = '\0';
                    strcat(cwd,filebuf);
                    FILE *readfile = fopen(cwd,"r");
                    if(readfile==NULL)
                    {
                        char x[6];
                        strcpy(x,"error");
                        x[5] = '\0';
                        send(connect,x,6,0);
                        printf("No such file\n");
                    }
                    else
                    {
                        printf("Sending the file to client\n");
                        char transmit_buffer[300];
                        int trans_len = 0;
                        explicit_bzero(&transmit_buffer,sizeof(transmit_buffer));
                        while((trans_len = fread(transmit_buffer,sizeof(char),256,readfile)) > 0)
                        {
                            send(connect,transmit_buffer,trans_len,0);
                            if(trans_len < 256)
                            {
                                if (feof(readfile))
                                {
                                    printf("End file\n");
                                }
                                if (ferror(readfile))
                                {
                                    printf("Error reading file\n");
                                }
                                break;
                            }
                            explicit_bzero(&transmit_buffer,sizeof(transmit_buffer));
                        }
                        printf("File sent successfully!\n");
                        fclose(readfile);
                    }
            }
            exit(EXIT_SUCCESS);
        }
        else
        {
           close(connect);
        }
        
       // sleep(1);
    }
return 0;
}
