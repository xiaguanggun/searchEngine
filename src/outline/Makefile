INCLUDES:=../../include
SRCS:=$(wildcard *.cpp) 
LIBS:=  -llog4cpp -lpthread
OBJS:=$(patsubst %.cpp, %.o, $(SRCS))
SERVER:=../../bin/outline

# 目标 : 依赖
$(SERVER): $(OBJS)
	@#g++ $^ -o $@ $(LIBS) $(addprefix -I, $(INCLUDES)) -g
	g++ $^ -o $@ $(LIBS) -g

%.o:%.cpp
	@#g++ -c $^ -o $@ $(addprefix -I, $(INCLUDES)) -g
	g++ -c $^ -o $@ -g

.PHONY:echo clean
echo:
	echo $(INCLUDES)
	echo $(SRCS)

clean:
	rm -rf $(OBJS) $(SERVER)
