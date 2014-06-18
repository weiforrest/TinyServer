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

#include <exception>
#include "log.h"
#include "mgr.h"

using std::pair;

int mgr::m_epollfd = -1;

mgr::mgr(int epollfd)
{
    m_epollfd = epollfd;
    int ret = 0;
	for(int i = 0; i < m_conncnt; i++){
		http_conn* tmp = NULL;
		try{
			tmp = new http_conn;
		}catch( ... ){
			log(LOG_ERR, __FILE__, __LINE__, "%s", "new http_conn failure");
			continue;
		}
		m_reserved.insert( pair<int, http_conn*>(-1, tmp));
	}
}

mgr::~mgr()
{
	map< int, http_conn* >::iterator it =  m_reserved.begin();
	for(;it != m_reserved.end(); ++it){
		delete it->second;
	}
	it = m_used.begin();
	for(;it != m_used.end(); ++it){
		delete it->second;
	}
}

int mgr::get_used_conn_cnt()
{
    return m_used.size();
}

http_conn* mgr::pick_conn(int cltfd )
{
	http_conn* tmp = NULL;
    if(m_reserved.empty()){
        log(LOG_INFO, __FILE__, __LINE__, "%s", "not enough connection to accept");
		try{
			tmp = new http_conn;
		}catch( ... ){
			log(LOG_ERR, __FILE__, __LINE__, "%s", "create new http_conn failure");
			return NULL;
		}
    }else{
		map< int, http_conn* >::iterator iter =  m_reserved.begin();
		tmp = iter->second;
		m_reserved.erase(iter);
	}
    if(!tmp){
        log(LOG_ERR, __FILE__, __LINE__, "%s", "empty connection object");
        return NULL;
    }
	
	tmp->acceptsock(cltfd);
    m_used.insert(pair< int, http_conn* >(cltfd, tmp));
    add_read_fd(m_epollfd, cltfd);
    log(LOG_INFO, __FILE__, __LINE__, "accept new sock %d", cltfd);
    return tmp;
}

void mgr::free_conn(http_conn* connection)
{
    int cltfd = connection->m_sockfd;
    closefd(m_epollfd, cltfd);
    m_used.erase(cltfd);
    connection->reset();
    m_reserved.insert(pair< int, http_conn* >(-1, connection));
}


RET_CODE mgr::process(int fd, OP_TYPE type)
{
    http_conn* connection = m_used[fd];
    if(!connection){
		log(LOG_ERR, __FILE__, __LINE__, "not found the connection fd = %d", fd);
        return NOTHING;
    }
	switch(type){
	case READ:{
		RET_CODE res = connection->read();
		switch(res){
		case OK:{
			log(LOG_DEBUG, __FILE__, __LINE__, "content read from client: %s", connection->m_read_buf);
		}
		case BUFFER_FULL:{
			modfd(m_epollfd, fd, EPOLLOUT);
			break;
		}
		case IOERR:
		case CLOSED:{
			free_conn(connection);
			return CLOSED;
		}
		default:
			break;
		}
	}
	case WRITE:{
		RET_CODE res = connection->write();
		switch(res){
		case TRY_AGAIN:{
			modfd(m_epollfd, fd, EPOLLOUT);
			break;
		}
		case BUFFER_EMPTY:{
			// 因为缓冲区满了，所以可能有没有读取完的数据，手动设置事件，继续读取
			modfd(m_epollfd, fd, EPOLLIN);
			break;
		}
		case IOERR:
		case CLOSED:{
			free_conn(connection);
			return CLOSED;
		}
		default:
			break;
		}
		break;
	}
	default:{
		log(LOG_ERR, __FILE__, __LINE__, "%s", "other operation not support yet");
		break;
	}
	}
	return OK;
}