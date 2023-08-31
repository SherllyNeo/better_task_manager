CC=gcc
C_FLAGS=-std=gnu11 -Wextra -Wall -pedantic
SRC_FILES=config.h src_files/dates.h src_files/task_manager_file.h
DEPS=config.h src_files/dates.h src_files/task_manager_file.h

better_task_manager: main.c $(SRC_FILES)
	$(CC) $(C_FLAGS) main.c $(DEPS) -o btm

install:
	$(CC) $(C_FLAGS) main.c $(DEPS) -o btm && cp ./btm $(HOME)/.local/bin

