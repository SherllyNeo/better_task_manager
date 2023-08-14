#ifndef TM_H
#define TM_H
#include "task_manager_file.c"

void display_tasks(char *task_path);
void display_upcoming(char *task_path);
int  add_task(char *task_name, char *task_text, char *date, char *task_path);
int remove_task_by_name(char *filepath, char *task_to_remove);

#endif
