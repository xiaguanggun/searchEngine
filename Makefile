.PHONY:outline online srpc all clean
all:outline online srpc
outline:
	cd src/outline && $(MAKE)
online:
	cd src/online && $(MAKE) 
srpc:
	cd src/srpc && $(MAKE)
clean:
	cd src/outline && $(MAKE) clean
	cd src/online && $(MAKE) clean
	cd src/srpc && $(MAKE) clean
