.PHONY:outline online all clean
outline:
	cd src/outline && $(MAKE)
online:
	cd src/online && $(MAKE) 
all:outline online
clean:
	cd src/outline && $(MAKE) clean
	cd src/online && $(MAKE) clean
