/* log.h    (c) 2014 weiforrest */
#ifndef __TINYSERVER_LOG_H_
#define __TINYSERVER_LOG_H_

#include <syslog.h>
#include <cstdarg>

void set_loglevel( int log_level = LOG_DEBUG );
void log( int log_level, const char* file_name, int line_num, const char* format, ... );

#endif	/* __TINYSERVER_LOG_H_ */
