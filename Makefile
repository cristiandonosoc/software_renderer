CC=gcc
CC_FLAGS=-pthread -g --std=gnu99
LIBS=-lm -lX11

CXX=g++
CXX_FLAGS=-Wall -shared -fPIC

MAKE=make

BIN_DIR=bin
TARGET=$(BIN_DIR)/renderer

CIMGUI_DIR=cimgui/cimgui
CIMGUI_SO=$(CIMGUI_DIR)/cimgui.so

all: renderer

renderer: $(BIN_DIR) $(CIMGUI_SO)
	$(CC) $(CC_FLAGS) -o $(TARGET) linux_main.c $(CIMGUI_SO) $(LIBS)

$(CIMGUI_SO):
	cd $(CIMGUI_DIR) && $(MAKE)

.cpp.o:
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

clean: 
	rm -f $(TARGET)
	cd $(CIMGUI_DIR) && $(MAKE) clean

$(BIN_DIR):
	mkdir -p $(BIN_DIR)


