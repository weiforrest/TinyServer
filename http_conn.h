/* http_conn.h    (c) 2014 weiforrest */
#ifndef __TINYSERVER_HTTP_CONN_H_
#define __TINYSERVER_HTTP_CONN_H_
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include "log.h"
#include "fdwrapper.h"
class http_conn
{
public:
    static const int FILENAME_LEN = 200;
    static const int READ_BUFFER_SIZE = 2048;
    static const int WRITE_BUFFER_SIZE = 1024;
    enum METHOD { GET = 0, POST, HEAD, PUT, DELETE, TRACE, OPTIONS, CONNECT, PATCH };
    enum CHECK_STATE { CHECK_STATE_REQUESTLINE = 0, CHECK_STATE_HEADER, CHECK_STATE_CONTENT };
    enum HTTP_CODE { NO_REQUEST, GET_REQUEST, BAD_REQUEST, NO_RESOURCE, FORBIDDEN_REQUEST, FILE_REQUEST, INTERNAL_ERROR, CLOSED_CONNECTION };
    enum LINE_STATUS { LINE_OK = 0, LINE_BAD, LINE_OPEN };

public:
    http_conn()
	{
		 m_read_buf = new char[READ_BUFFER_SIZE];
		 m_write_buf = new char[WRITE_BUFFER_SIZE];
		 m_real_file = new char[FILENAME_LEN];
		 reset();
	}
	
    ~http_conn()
	{
		 delete [] m_read_buf;
		 delete [] m_write_buf;
		 delete [] m_real_file;
	}

public:
    void init(int sockfd, const sockaddr_in& addr);
    void reset();
	HTTP_CODE process();
	RET_CODE read();
	RET_CODE write();
	void acceptsock(int sockfd)
	{
		 if(m_sockfd != -1)
			  log(LOG_ERR, __FILE__, __LINE__, "%s", "http_conn have a socket already");
		 m_sockfd = sockfd;
	}
private:
    void init();
	bool process_read();
    bool process_write(HTTP_CODE ret);

    HTTP_CODE parse_request_line(char* text);
    HTTP_CODE parse_headers(char* text);
    HTTP_CODE parse_content(char* text);
    HTTP_CODE do_request();
    char* get_line() { return m_read_buf + m_start_line; }
    LINE_STATUS parse_line();

    void unmap();
    bool add_response(const char* format, ...);
    bool add_content(const char* content);
    bool add_status_line(int status, const char* title);
    bool add_headers(int content_length);
    bool add_content_length(int content_length);
    bool add_linger();
    bool add_blank_line();

public:
    int m_sockfd;
private:

    char * m_read_buf;
    int m_read_idx;
	int m_checked_idx;
	
    char * m_write_buf;
    int m_write_idx;
	
    int m_start_line;
    CHECK_STATE m_check_state;
    METHOD m_method;

    char * m_real_file;
	
    char* m_url;
    char* m_version;
    char* m_host;
    int m_content_length;

	char * m_file_address;
    struct stat m_file_stat;
    struct iovec m_iv[2];
    int m_iv_count;
};

#endif	/* __TINYSERVER_HTTP_CONN_H_ */
