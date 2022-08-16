#if defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#endif

#if defined(_WIN32)
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define CLOSESOCKET(s) closesocket(s)
#define GETSOCKETERRNO() WSAGetLastError()

#else
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
#endif


#include <stdio.h>
#include <string.h>
#include <time.h>
#include<iostream>
using namespace std;
#if defined(_WIN32)
#include<conio.h>
#endif // defined
class packet
{
public:
    int a;
    char message[1000];
    packet()
    {

    }
    packet(int aa,char mess[1000])
    {
        strcpy(message,mess);
        a=aa;
    }
};
int main()
{
    #if defined(_WIN32)
    WSADATA d;
    if(WSAStartup(MAKEWORD(2,2),&d))
    {
        cout<<"\n failed to initialize";
    }
    #endif // defined
    struct addrinfo hints;
    memset(&hints,0,sizeof(hints));
    hints.ai_socktype=SOCK_STREAM;

    hints.ai_flags=AI_PASSIVE;

    struct addrinfo *bind_address;
    getaddrinfo(0,"8080",&hints,&bind_address);
    SOCKET socket_listen=socket(bind_address->ai_family,bind_address->ai_socktype,bind_address->ai_protocol);
    if(!ISVALIDSOCKET(socket_listen))
    {
        cout<<"\n socket not created==>"<<GETSOCKETERRNO();
    }

    int option=0;
    if(setsockopt(socket_listen,IPPROTO_IPV6,IPV6_V6ONLY,(char*)&option,sizeof(option)))
    {
        cout<<"\n problem in setting the flag==>";
    }

    cout<<"\n binding the socket==>";
    if(bind(socket_listen,bind_address->ai_addr,bind_address->ai_addrlen))
    {
        cout<<"\n bind failed==>"<<GETSOCKETERRNO();
    }
    if(listen(socket_listen,20)<0)
    {
        cout<<"\n socket failed";
    }
    freeaddrinfo(bind_address);
    fd_set master;
    FD_ZERO(&master);
    FD_SET(socket_listen,&master);
    SOCKET max_socket=socket_listen;
    cout<<"\n socket number of socket_listen is==>"<<socket_listen;
    while(1)
    {
        fd_set read=master;
        if(select(max_socket+1,&read,0,0,0)<0)
        {
            cout<<"\n select failed";
        }
        for(SOCKET i=1;i<=max_socket;i++)
        {
            if(FD_ISSET(i,&read))
            {
                if(i==socket_listen)//for accepting new connections
                {
                    struct sockaddr_storage client_address;
                    socklen_t client_len=sizeof(client_address);
                    SOCKET client_socket=accept(socket_listen,(struct sockaddr*)&client_address,&client_len);
                    if(!ISVALIDSOCKET(client_socket))
                    {
                        cout<<"\n client socket creation failure==>"<<GETSOCKETERRNO();
                    }
                    FD_SET(client_socket,&master);
                    if(client_socket>max_socket)
                        max_socket=client_socket;
                    //getting the ip address of the stuff
                    char add_buff[1000];
                    getnameinfo((struct sockaddr*)&client_address,client_len,add_buff,sizeof(add_buff),0,0,NI_NUMERICHOST);
                    cout<<"\n the ip address of the new user is==>"<<add_buff;
                }
                /*
                else {
                    char read[1024];
                    int bytes_received = recv(i, read,1024, 0);
                    if (bytes_received < 1) {
                        FD_CLR(i, &master);
                        CLOSESOCKET(i);
                        continue;
                    }
                    SOCKET j;
                    for (j = 1; j <= max_socket; ++j) {
                        if (FD_ISSET(j, &master)) {
                            if (j == socket_listen || j == i)
                                continue;
                            else
                                send(j, read,bytes_received, 0);
                        }
                    }
                }
                */


                else//read data from the user and send it to all other clients
                {
                   packet rec;
                    memset(rec.message,0,sizeof(rec.message));

                    int bytes=recv(i,(char*)&rec,sizeof(rec),0);
                    //cout<<"\n received bytes are==>"<<bytes;
                    //cout<<"\n received are==>"<<buff;
                    //cout<<"\n received stuff is==>"<<buff;
                    if(bytes<1)
                    {
                        FD_CLR(i,&master);
                        CLOSESOCKET(i);
                        continue;
                    }
                    for(SOCKET j=1;j<=max_socket;j++)
                    {
                        if(FD_ISSET(j,&master))
                        {


                        if(j!=socket_listen&&j!=i)
                        {
                                     cout<<"\n sending to socket number==>"<<j<<endl;
                            //sending the data to all other sockets
                            int sent=send(j,(char*)&rec,bytes,0);
                      //      cout<<"\n bytes sent to other people are==>"<<sent;
                        }

                    }
                    }

                }


            }
        }

    }
        printf("Closing listening socket...\n");
    CLOSESOCKET(socket_listen);

#if defined(_WIN32)
    WSACleanup();
#endif


    printf("Finished.\n");


}
