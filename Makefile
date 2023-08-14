CC=gcc

better_task_manager: main.c config.h src_files/dates.h 
	$(CC) -std=gnu11 main.c config.h src_files/dates.h  -o btm

install:
	$(CC) -std=gnu11 main.c config.h src_files/dates.h  && cp ./btm /usr/local/bin

