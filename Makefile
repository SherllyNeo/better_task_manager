CC=gcc
C_FLAGS=-std=gnu11 -Wextra -Wall -pedantic
SRC=src/config.h src/dates.h src/task_manager_file.h
DEPS=src/config.h src/dates.h src/task_manager_file.h

better_task_manager: src/main.c $(SRC)
	mkdir -p bin
	$(CC) $(C_FLAGS) src/main.c $(DEPS) -o bin/btm

install:
	mkdir -p bin
	$(CC) $(C_FLAGS) src/main.c $(DEPS) -o bin/btm && cp bin/btm $(HOME)/.local/bin

