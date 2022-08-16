//tcp client code
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
    struct addrinfo *server_add;
    char buff[1000];
    getaddrinfo("172.16.50.25","8080",&hints,&server_add);
    getnameinfo(server_add->ai_addr,server_add->ai_addrlen,buff,sizeof(buff),0,0,NI_NUMERICHOST);
    cout<<"\n the server address is==>"<<buff;
    cout<<endl;
    SOCKET peer_socket=socket(server_add->ai_family,server_add->ai_socktype,server_add->ai_protocol);
    cout<<"\n socket number of the user is==>"<<peer_socket;
    if(!ISVALIDSOCKET(peer_socket))
    {
        cout<<"\n socket not created==>"<<GETSOCKETERRNO();
    }
    if(connect(peer_socket,server_add->ai_addr,server_add->ai_addrlen))
    {
        cout<<"\n problem in connecting";
    }
    freeaddrinfo(server_add);
    fd_set master;
    FD_ZERO(&master);
    FD_SET(peer_socket,&master);
    SOCKET max_socket=peer_socket;
    cout<<"\n enter the message to be sent==>";
    while(1)
    {
        fd_set read=master;
        struct timeval time;
        time.tv_sec=0;
        time.tv_usec=10000;
        if(select(peer_socket+1,&read,0,0,&time)<0)
        {
            cout<<"\n select failed";
        }
        if(FD_ISSET(peer_socket,&read))
        {
            packet rec;

            memset(rec.message,0,sizeof(rec.message));
            //receiving the stuff and printing it to the screen....
            int bytes=recv(peer_socket,(char*)&rec,sizeof(rec),0);
            if(bytes<1)
            {
                cout<<"\n connection closed by the server";
                CLOSESOCKET(peer_socket);
                break;
            }
            cout<<"\n bytes received are==>"<<bytes;
            cout<<"\n message is==>";
            cout<<rec.a<<" "<<rec.message;
        }
        if(_kbhit())
        {
            cout<<"\n enter the number==>";
            packet p;
            cin>>p.a;
            fflush(stdin);
            cout<<"\n enter the message==>";

            gets(p.message);

            int bytes=send(peer_socket,(char*)&p,sizeof(p),0);
            if(bytes==sizeof(p))
            {
                cout<<"\n bytes sent!\n";
            }
        }
    }
    printf("Closing socket...\n");

#if defined(_WIN32)
 WSACleanup();
#endif
 printf("Finished.\n");
 return 0;

}
