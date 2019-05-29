# This makefile was generated on Wed 29 May 2019 01:59:27 PM CEST by mkmake.sh
# See https://github.com/teo8192/mkmake.git for more details.
EXECUTABLEheap=bin/heap
DEBUGheap=bin/heap_debug

CC=gcc

FLAGS=

COMP_FLAGS=

.PHONY: all
all: bin objs $(EXECUTABLEheap) 

.PHONY: debug
debug: $(DEBUGheap) 

.PHONY: clean
clean:
	@rm -f objs/*.o $(DEBUGheap) $(EXECUTABLEheap) 
	@echo "cleaned"

.PHONY: dbclean
dbclean:
	@rm -f objs/debug_[0-9a-zA-Z_]*.o $(DEBUGheap) 
	@echo "cleaned debug"

bin:
	@[ ! -d bin ] && mkdir bin

objs:
	@[ ! -d objs ] && mkdir objs

objs/main.o: main.c 
	@echo "compiling main.c"
	@$(CC) -c main.c -O2 $(COMP_FLAGS)
	@mv main.o objs

objs/debug_main.o: main.c 
	@echo "compiling main.c with debug flag"
	@$(CC) -c main.c -g -o debug_main.o $(COMP_FLAGS)
	@mv debug_main.o objs

objs/heap.o: heap.c heap.h plot.h
	@echo "compiling heap.c"
	@$(CC) -c heap.c -O2 $(COMP_FLAGS)
	@mv heap.o objs

objs/debug_heap.o: heap.c heap.h plot.h
	@echo "compiling heap.c with debug flag"
	@$(CC) -c heap.c -g -o debug_heap.o $(COMP_FLAGS)
	@mv debug_heap.o objs

objs/plot.o: plot.c plot.h
	@echo "compiling plot.c"
	@$(CC) -c plot.c -O2 $(COMP_FLAGS)
	@mv plot.o objs

objs/debug_plot.o: plot.c plot.h
	@echo "compiling plot.c with debug flag"
	@$(CC) -c plot.c -g -o debug_plot.o $(COMP_FLAGS)
	@mv debug_plot.o objs

$(EXECUTABLEheap): objs/heap.o objs/main.o objs/plot.o 
	@echo "linking heap"
	
	@$(CC) -o $@ objs/heap.o objs/main.o objs/plot.o $(FLAGS)

$(DEBUGheap): objs/debug_heap.o objs/debug_main.o objs/debug_plot.o 
	@echo "linking debug_heap"
	@$(CC) -o $@ objs/debug_heap.o objs/debug_main.o objs/debug_plot.o $(FLAGS)

