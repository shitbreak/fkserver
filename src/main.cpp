#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/errno.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>

void checkerror()
{
    if (errno > 0) {
        printf("socket api error:%d", errno);
        getchar();
    }
}

void doit(int connfd)
{
    char buff[512 + 1];
    time_t ticks = time(NULL);
    snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
    write(connfd, buff, strlen(buff));
    
    struct sockaddr_in local,remote;
    socklen_t len  = sizeof(sockaddr);
    getsockname(connfd,(sockaddr*)&local,&len);
    getpeername(connfd,(sockaddr*)&remote,&len);
    
    char str_local_addr[INET_ADDRSTRLEN] = {0};
    int port_local = 0;
    char str_remote_addr[INET_ADDRSTRLEN] = {0};
    int port_remote = 0;
    
    if(inet_ntop(AF_INET,&local.sin_addr,str_local_addr,sizeof(str_local_addr)) == NULL)
        printf("inet_ntop error!");
    if(inet_ntop(AF_INET,&remote.sin_addr,str_remote_addr,sizeof(str_remote_addr)) == NULL)
        printf("inet_ntop error!");
        
     port_local = ntohs(local.sin_port);
     port_remote = ntohs(remote.sin_port);
     
     printf("%s:%d connnected and %s:%d answered\n",
     str_remote_addr,port_remote,str_local_addr,port_local);
}

int main()
{
    int listenfd, connfd;
    pid_t pid = 0;
    
    struct sockaddr_in servaddr;
    struct sockaddr_in clitaddr;
   

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket error!");
        return 0;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(60000);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));
    checkerror();
    listen(listenfd, 1024);
    checkerror();
    printf("server is listening at port:60000\n");
    for (;;) {
        int len = sizeof(clitaddr);
        connfd = accept(listenfd,(sockaddr*)&clitaddr, (socklen_t*)&len);
        if((pid == fork()) != 0)
        {
            close(listenfd);
            doit(connfd);
            close(connfd);
            exit(0);
        }
        close(connfd);
    }
    return (0);
}
