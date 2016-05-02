all: dir
	gcc linux_main.c -lX11 -pthread -g --std=gnu99 -o build/renderer
dir:
	mkdir -p build
