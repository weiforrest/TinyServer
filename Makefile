#####################################################
# Makefile For TingServet 	(c) 2014 weiforrest
#####################################################

#Programs
CXX = g++

#FLAGS
CPPFLAGS = -c -g -o

#Targets
OBJ = log.o fdwrapper.o http_conn.o mgr.o main.o
FINISH = tinyserver
.cpp.o:
	$(CXX) $(CPPFLAGS) $@ $<
all: $(OBJ) $(FINISH)


.PHONY: clean all

$(FINISH): $(OBJ)
	$(CXX) -g -o $@ $^

clean:
	rm -rf $(OBJ) $(FINISH)
