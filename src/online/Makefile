OUTLINE:=../outline
OUT:=../../bin/webQuery
LIBS:= -llog4cpp -lpthread
SRCS:=$(wildcard *.cpp)
OBJS:=$(patsubst %.cpp,%.o,$(SRCS))
OUTLINESRCS:=$(wildcard $(OUTLINE)/*.cpp)
OUTLINEOBJS:=$(patsubst $(OUTLINE)/%.cpp,$(OUTLINE)/%.o,$(OUTLINESRCS))
EXCLUDE_OBJS:=$(OUTLINE)/test.o
CC:=g++
$(OUT):$(OBJS) $(filter-out $(EXCLUDE_OBJS),$(OUTLINEOBJS))
	$(CC) $^ -o $@ $(LIBS) -g
%.o:%.cpp
	$(CC) -c $^ -o $@ -g
$(OUTLINE)/%.o:$(OUTLINE)/%.cpp
	$(CC) -c $^ -o $@ -g
.PHONY:clean rebuild
clean:
	$(RM) $(OUT) $(OBJS) $(OUTLINEOBJS)
rebuild:clean $(OUT)
