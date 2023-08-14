CC=gcc

better_task_manager: main.c config.h src_files/dates.h src_files/task_manager_file.h
	$(CC) -std=gnu11 main.c config.h src_files/dates.h src_files/task_manager_file.h -o btm

install:
	$(CC) -std=gnu11 main.c config.h src_files/dates.h src_files/task_manager_file.h && cp ./btm /usr/local/bin

