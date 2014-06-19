/* mgr.h    (c) 2014 weiforrest */
#ifndef __TINYSERVER_MGR_H_
#define __TINYSERVER_MGR_H_

#include <map>
#include <list>
#include <iterator>
#include <arpa/inet.h>
#include "fdwrapper.h"
#include "http_conn.h"

using std::map;
using std::pair;
/* using std::iterator; */
using std::list;
class mgr
{
public:
    mgr(int epollfd);
    ~mgr();

    http_conn* pick_conn(int sockfd); /* 获取连接 */
    void free_conn(http_conn* connection);
    int get_used_conn_cnt();	/* 当前已经使用的连接 */
    RET_CODE process(int fd, OP_TYPE type);

private:
    static int m_epollfd;
    list< pair<int, http_conn*> > m_reserved;
    map<int, http_conn*> m_used;
	static const int m_conncnt = 10;
};

#endif	/* __TINYSERVER_MGR_H_ */
