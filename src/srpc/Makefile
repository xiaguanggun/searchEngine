REACTOR:=../reactor
SHARED:=../shared
CACHE:=../cache
ONLINE:=../online
SERVER:=../../bin/apiGateway
KeyWordService:=../../bin/KeyWordService
WebQueryService:=../../bin/WebQueryService
FLAGS:= -Wall
LIBS:=  -lwfrest -lworkflow -lcrypt \
		-lalibabacloud-oss-cpp-sdk -lssl -lcrypto \
		-lcurl -lpthread -lrabbitmq -lSimpleAmqpClient \
		-lsrpc -lprotobuf -llz4 -lsnappy \
		-lppconsul -ll8w8jwt -llog4cpp
SRCS:=$(wildcard *.cc) 
OBJS:=$(patsubst %.cc, %.o, $(SRCS))
SHAREDSRCS:=$(wildcard $(SHARED)/*.cpp)
SHAREDOBJS:=$(patsubst $(SHARED)/%.cpp,$(SHARED)/%.o,$(SHAREDSRCS))
REACTORSRCS:=$(wildcard $(REACTOR)/*.cpp)
REACTOROBJS:=$(patsubst $(REACTOR)/%.cpp,$(REACTOR)/%.o,$(REACTORSRCS))
CACHESRCS:=$(wildcard $(CACHE)/*.cpp)
CACHEOBJS:=$(patsubst $(CACHE)/%.cpp,$(CACHE)/%.o,$(CACHESRCS))
ONLINESRCS:=$(wildcard $(ONLINE)/*.cpp)
ONLINEOBJS:=$(patsubst $(ONLINE)/%.cpp,$(ONLINE)/%.o,$(ONLINESRCS))
CC:=g++
ifdef DEBUG
	FLAGS += -g -DDEBUG
endif
# 目标 : 依赖
all:$(SERVER) $(KeyWordService) $(WebQueryService)
$(SERVER):main.o service.pb.o APIGateway.o
	$(CC) $^ -o $@ $(LIBS) $(FLAGS)
$(KeyWordService):$(filter-out main.o WebQueryService.o,$(OBJS)) $(SHAREDOBJS) $(ONLINE)/Dictionary.o
	$(CC) $^ -o $@ $(LIBS) $(FLAGS)
$(WebQueryService):$(filter-out main.o KeyWordService.o,$(OBJS)) $(SHAREDOBJS) $(ONLINE)/WebPageQuery.o
	$(CC) $^ -o $@ $(LIBS) $(FLAGS)

.PHONY:all service clean rebuild
service:$(KeyWordService) $(WebQueryService)
clean:
	$(RM) $(OBJS) $(SERVER) $(KeyWordService) $(WebQueryService) $(SHAREDOBJS) $(REACTOROBJS) $(CACHEOBJS) $(ONLINEOBJS)
rebuild:clean $(SERVER)
