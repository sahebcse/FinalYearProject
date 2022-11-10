current_dir = $(shell pwd)

default:
	gcc -I$(current_dir)/include $(current_dir)/src/*.c $(current_dir)/main.c -o output -lm

cook-headers:
	gcc -I$(current_dir)/include $(current_dir)/include/*.h
	@echo "Precompiled headers created!"
