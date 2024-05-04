SHARED:=../shared
OUT:=../../bin/outline
LIBS:= -llog4cpp -lpthread
SRCS:=$(wildcard *.cpp) 
OBJS:=$(patsubst %.cpp, %.o, $(SRCS))
SHAREDSRCS:=$(wildcard $(SHARED)/*.cpp)
SHAREDOBJS:=$(patsubst $(SHARED)/%.cpp,$(SHARED)/%.o,$(SHAREDSRCS))
EXCLUDE_OBJS:=$(SHARED)/webquery.o
CC:=g++
# 目标 : 依赖
$(OUT):$(OBJS) $(filter-out $(EXCLUDE_OBJS),$(SHAREDOBJS))
	$(CC) $^ -o $@ $(LIBS) -g
%.o:%.cpp
	$(CC) -c $^ -o $@ -g
$(SHARED)/%.o:$(SHARED)/%.cpp
	$(CC) -c $^ -o $@ -g

.PHONY:clean rebuild
clean:
	$(RM) $(OUT) $(OBJS) $(SHAREDOBJS)
rebuild:clean $(OUT)
