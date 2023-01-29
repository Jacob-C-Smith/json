INCLUDES=-I./include -I/usr/local/include -I./dict/include
CC=gcc
CFLAGS=-g -fdeclspec -fms-extensions -w
OBJECT_DIR=./object
AS=nasm
SRCS:=$(wildcard *.c)
OBJS=$(SRCS:.c=.o) 
TARGET=JSON
SLIBTARGET=lib$(TARGET).a
SUBMODULES:=dict
LDIRS=-L./
LIBS=-ldict
SOTARGET=lib$(TARGET).so
RUNARGS=example.json

.PHONY: all
.PHONY: depend
.PHONY: clean
.PHONY: clean_submodules
.PHONY: submodules

all: $(TARGET) submodules libs

$(TARGET): submodules $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LDIRS) $(LIBS)

submodules: $(SUBMODULES)
	$(MAKE) libs -C $^
	cp $(SUBMODULES)/lib$(SUBMODULES).so ./lib$(SUBMODULES).so
	cp $(SUBMODULES)/lib$(SUBMODULES).a ./lib$(SUBMODULES).a

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@ 

clean: clean_submodules
	rm *.o $(TARGET) *.a *.so || true

clean_submodules: $(SUBMODULES)
	$(MAKE) clean -C $^

run: $(TARGET)
	./$(TARGET) $(RUNARGS)

libs: $(OBJS)
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $(OBJS) $(LDIRS) $(LIBS)
	ar -rc $(SLIBTARGET) $(TARGET).o || true
	$(CC) $(CFLAGS) -fPIC $(INCLUDES) -o $(SLIBTARGET) $(OBJS) $(LDIRS) $(LIBS)
	$(CC) -shared -o $(SOTARGET) $(OBJS) $(LDIRS) $(LIBS)

depend: $(SRCS)
	makedepend $(INCLUDES) $^
	

