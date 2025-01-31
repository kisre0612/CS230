#include <string.h>
#include <stdio.h>
#include "csapp.h"
//free-port.sh -> 4500
//port-for-user.pl -> 23734
/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

void *thread(void *vargp);
void doit(int fd);
//void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
void parse_uri(char *uri, char *host, char *path, int *port);
void build_reqhdrs(rio_t *rp, char *serverreq, char *host, char *port);

int main(int argc, char **argv) {
    int listenfd;
    //int port = atoi(argv[1]);
    char hostname[MAXLINE], port[MAXLINE];
    struct sockaddr_storage clientaddr;
    socklen_t clientlen = sizeof(clientaddr);
    pthread_t tid;

    /* Check command line args */
    if (argc != 2) {
	    fprintf(stderr, "usage: %s <port>\n", argv[0]);
	    exit(1);
    }
    listenfd = Open_listenfd(argv[1]);
    while(1) {
        int *connfdp = Malloc(sizeof(int));
        *connfdp = Accept(listenfd, (SA *) &clientaddr, &clientlen);
        Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
        printf("Connected to (%s, %s)\n", hostname, port);
        Pthread_create(&tid, NULL, thread, connfdp);
        //Close(connfdp); 필요 x
    }
}

void *thread(void *vargp) { //thread routine 돌기
    int connfd = *((int *) vargp);
    Pthread_detach(pthread_self());
    Free(vargp);
    doit(connfd); //책의 thread 함수에서 echo -> doit
    Close(connfd);
    return NULL;
}

/*
 * doit - handle one HTTP request/response transaction
 */
void doit(int fd) {
    int serverfd, port, n;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char hostname[MAXLINE], path[MAXLINE]; //  /hub/index.html 부분 담기(파일 경로)
    char portstr[8], request[MAXLINE]; //port string type형태, server에서 받는 request
    rio_t client, server;

    Rio_readinitb(&client, fd); //header랑 request line 읽기
    if (!Rio_readlineb(&client, buf, MAXLINE)) return;
    sscanf(buf, "%s %s %s", method, uri, version);       
    // if (strcasecmp(method, "GET")) {                     
    //     clienterror(fd, method, "501", "Not Implemented", "Proxy Server does not implement this method");
    //     return;
    // }

    //parse uri then open a clientfd
    parse_uri(uri, hostname, path, &port);
    sprintf(portstr, "%d", port);
    serverfd = Open_clientfd(hostname, portstr);
    // if(serverfd < 0){
    //     printf("Connection Failed\n");
    //     return;
    // }
    Rio_readinitb(&server, serverfd);

    //server에 주는 request : GET /hub/index.html HTTP/1.0 꼴
    sprintf(request, "GET %s HTTP/1.0\r\n", path); 
    build_reqhdrs(&client, request, hostname, &port);
    Rio_writen(serverfd, request, strlen(request)); //client header를 real server로 보낸다고 생각
    while ((n = Rio_readlineb(&server, buf, MAXLINE))) {//real server -> buf 반응
    //    printf("Proxy received %d bytes\n", n);
        Rio_writen(fd, buf, n);  //real server -> real client 반응
    }
}

/*
 * clienterror - returns an error message to the client
 */
// void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg) {
//     char buf[MAXLINE], body[MAXBUF];

//     /* Build the HTTP response body */
//     sprintf(body, "<html><title>Proxy Error</title>");
//     sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
//     sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
//     sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
//     sprintf(body, "%s<hr><em>The Proxy Web server</em>\r\n", body);

//     /* Print the HTTP response */
//     sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
//     Rio_writen(fd, buf, strlen(buf));
//     sprintf(buf, "Content-type: text/html\r\n");
//     Rio_writen(fd, buf, strlen(buf));
//     sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
//     Rio_writen(fd, buf, strlen(buf));
//     Rio_writen(fd, body, strlen(body));
// }

/*
 * parse_uri - parse URI into host and path
 */
void parse_uri(char *uri, char *host, char *path, int *port) {
    //uri 예시 http://www.cmu.edu:8080/hub/index.html 생각
    //strstr -> 문자열 안에서 문자열 찾는 함수. 찾았으면 문자열로 시작하는 문자열의 포인터 반환, 없으면 NULL 반환
    char *ptr1 = strstr(uri, "//");
    if(ptr1 != NULL) ptr1 = ptr1 + 2;
    else ptr1 = uri;
    //ptr1 = www.cmu.edu:8080/hub/index.html
    char *ptr2 = strstr(ptr1, ":");
    //ptr2 = :8080/hub/index.html
    if(ptr2 == NULL) { //:8080과 같은 부분이 없을 때
        ptr2 = strstr(ptr1, "/");
        //ptr2 = /hub/index.html
        if(ptr2 == NULL) { //ptr1 = www.cmu.edu로 끝나는 경우
            strncpy(host, ptr1, MAXLINE);
            strcpy(path, "");
            return;
        }
        *ptr2 = '\0';
        strncpy(host, ptr1, MAXLINE);
        *ptr2 = '/';
        strncpy(path, ptr2, MAXLINE);
    } else { //:8080과 같은 부분이 있을 때
        *ptr2 = '\0'; //ptr1 = www.cmu.edu/08080/hub/index.html
        strncpy(host, ptr1, MAXLINE);
        sscanf(ptr2 + 1, "%d%s", port, path); //ptr2 + 1 = 8080/hub/index.html로 하기
    }
}

/*
 * build_reqhdrs - build HTTP request headers
 */
void build_reqhdrs(rio_t *rp, char *request, char *host, char *port) {
    char buf[MAXLINE];
    while(Rio_readlineb(rp, buf, MAXLINE) > 0) {
        if(strcmp(buf, "\r\n") == 0) break;         //line:netp:readhdrs:checkterm
        sprintf(request, "%s%s", request, buf);
    }
    sprintf(request, "%sHost: %s:%s\r\n", request, host, port);
    sprintf(request, "%s%s%s%s", request, user_agent_hdr, "Connection: close\r\n", "Proxy-Connection: close\r\n");
    sprintf(request, "%s\r\n", request);
    return;
}