.PHONY:outline online all clean
all:outline online
outline:
	cd src/outline && $(MAKE)
online:
	cd src/online && $(MAKE) 
clean:
	cd src/outline && $(MAKE) clean
	cd src/online && $(MAKE) clean
