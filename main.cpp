#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <vector>

#include "log.h"
#include "http_conn.h"
#include "mgr.h"
#include "processpool.h"

using std::vector;

static void usage(const char* prog)
{
    log(LOG_INFO, __FILE__, __LINE__,  "usage: %s [-x] [-h] [-p port] [-a ip_addr]", prog);
}

int main(int argc, char* argv[])
{
	char ip[100];
	int port;
    memset( ip, '\0', 100);
    int option;
    while ((option = getopt(argc, argv, "xa:p:h")) != -1){
        switch (option){
     		case 'x':{
                set_loglevel(LOG_DEBUG);
                break;
            }
            case 'h':{
                usage(basename(argv[0]));
                return 0;
            }
		    case 'a':{
                memcpy(ip, optarg, strlen(optarg));
                break;
            }
		    case 'p':{
				port = atoi(optarg);
				break;
			}
		    case '?':{
                log(LOG_ERR, __FILE__, __LINE__, "un-recognized option %c", option);
                usage(basename(argv[0]));
                return 1;
            }
        }
    }    

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);
	// debug used
	int reuse = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(listenfd, 5);
    assert(ret != -1);

    processpool< mgr, http_conn >* pool = processpool< mgr, http_conn >::create(listenfd);
    if(pool){
        pool->run();
        delete pool;
    }
    close(listenfd);
    return 0;
	}
