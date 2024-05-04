REACTOR:=../reactor
SHARED:=../shared
OUT:=../../bin/webquery
LIBS:= -llog4cpp -lpthread
SRCS:=$(wildcard *.cpp)
OBJS:=$(patsubst %.cpp,%.o,$(SRCS))
SHAREDSRCS:=$(wildcard $(SHARED)/*.cpp)
SHAREDOBJS:=$(patsubst $(SHARED)/%.cpp,$(SHARED)/%.o,$(SHAREDSRCS))
REACTORSRCS:=$(wildcard $(REACTOR)/*.cpp)
REACTOROBJS:=$(patsubst $(REACTOR)/%.cpp,$(REACTOR)/%.o,$(REACTORSRCS))
CC:=g++
# 目标 : 依赖
$(OUT):$(OBJS) $(SHAREDOBJS) $(REACTOROBJS)
	$(CC) $^ -o $@ $(LIBS) -g
%.o:%.cpp
	$(CC) -c $^ -o $@ -g
$(SHARED)/%.o:$(SHARED)/%.cpp
	$(CC) -c $^ -o $@ -g
$(REACTOR)/%.o:$(REACTOR)/%.cpp
	$(CC) -c $^ -o $@ -g

.PHONY:clean rebuild
clean:
	$(RM) $(OUT) $(OBJS) $(SHAREDOBJS) $(REACTOROBJS)
rebuild:clean $(OUT)
