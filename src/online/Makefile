REACTOR:=../reactor
SHARED:=../shared
CACHE:=../cache
SERVER:=../../bin/server
KEYWORD:=../../bin/keyword
WEBQUERY:=../../bin/webquery
LIBS:= -llog4cpp -lpthread
SRCS:=$(wildcard *.cpp)
OBJS:=$(patsubst %.cpp,%.o,$(SRCS))
SHAREDSRCS:=$(wildcard $(SHARED)/*.cpp)
SHAREDOBJS:=$(patsubst $(SHARED)/%.cpp,$(SHARED)/%.o,$(SHAREDSRCS))
REACTORSRCS:=$(wildcard $(REACTOR)/*.cpp)
REACTOROBJS:=$(patsubst $(REACTOR)/%.cpp,$(REACTOR)/%.o,$(REACTORSRCS))
CACHESRCS:=$(wildcard $(CACHE)/*.cpp)
CACHEOBJS:=$(patsubst $(CACHE)/%.cpp,$(CACHE)/%.o,$(CACHESRCS))
CC:=g++
# 目标 : 依赖
$(SERVER):$(filter-out keyword.o webquery.o,$(OBJS)) $(SHAREDOBJS) $(REACTOROBJS) $(CACHEOBJS)
	$(CC) $^ -o $@ $(LIBS) -g
$(KEYWORD):$(filter-out server.o webquery.o,$(OBJS)) $(SHAREDOBJS) $(REACTOROBJS) $(CACHEOBJS)
	$(CC) $^ -o $@ $(LIBS) -g
$(WEBQUERY):$(filter-out keyword.o server.o,$(OBJS)) $(SHAREDOBJS) $(REACTOROBJS) $(CACHEOBJS)
	$(CC) $^ -o $@ $(LIBS) -g
%.o:%.cpp
	$(CC) -c $^ -o $@ -g
$(SHARED)/%.o:$(SHARED)/%.cpp
	$(CC) -c $^ -o $@ -g
$(REACTOR)/%.o:$(REACTOR)/%.cpp
	$(CC) -c $^ -o $@ -g
$(CACHE)/%.o:$(CACHE)/%.cpp
	$(CC) -c $^ -o $@ -g

.PHONY:all keyword webquery clean rebuild
all:$(KEYWORD) $(WEBQUERY)
keyword:$(KEYWORD)
webquery:$(WEBQUERY)
clean:
	$(RM) $(SERVER) $(KEYWORD) $(WEBQUERY) $(OBJS) $(SHAREDOBJS) $(REACTOROBJS) $(CACHEOBJS)
rebuild:clean $(SERVER)
