file tinyserver
set follow-fork-mode child

# 1
b processpool.h:203
commands 1
  if number > 0 
	printf "get epool  %d event\n", number
  end
  continue
end

# 2
b processpool.h:263
disable breakpoint 2

# 3
b processpool.h:264
commands 3
  printf "process fd = %d read result = ", sockfd
  print result
  continue
end

# 4
b processpool.h:273
disable breakpoint 4

# 5
b processpool.h:274
commands 5
  printf "process fd = %d write result = ", sockfd
  print result
  continue
end

# 6
b mgr.cpp:105
disable breakpoint 6

# 7
b mgr.cpp:106
commands 7
  printf "http_conn->read() return "
  print res
  continue
end

# 8
b mgr.cpp:129
disable breakpoint 8x

# 9
b mgr.cpp:130
commands 9
  printf "http_conn->write() return "
  print res
  continue
end

# 10
b http_conn.cpp:222
commands 10
  printf "http_conn::process return "
  print read_ret
end

# 11
b http_conn.cpp:308

# 12
b http_conn.cpp:336

# 13
b processpool.h:223
commands 13
  printf "accept the sockfd = %d \n", connfd
  continue
end

# 14
b http_conn.cpp:221
commands 14
  printf "http_conn::process_read return is ture\n"
  continue
end

# 15
b http_conn.cpp:232
commands 15
  printf "http_conn::process_read return false\n"
  continue
end
# run
r -p80 -a0.0.0.0

