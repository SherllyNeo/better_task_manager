#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "config.h"
#include "src_files/dates.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>

#define SIZE 1000

struct Task {
    char date[SIZE];
    char task_name[SIZE];
    char task_text[SIZE];
};

struct Task* csv_to_task_array(char* task_deck,int* p_amount_of_tasks) {
    int size_of_task_array = 50;
    struct Task* task_array = malloc(size_of_task_array*sizeof(struct Task));
    char buffer[SIZE];
    FILE *deck = NULL;
    int flag = 0;
    int lines = 0;
    deck = fopen(task_deck,"r");
    while (fgets(buffer, SIZE, deck)) {

        char *token = strtok(buffer, ",");
        /* Make a struct Task to add to task_array */
        struct Task tmp_task = {};
        while (token) {
            /* this will be 4 each time. The promt, answer, date and delay */
            int parse_helper = flag % 3;
            switch(parse_helper) {
                case 0:
                    ;
                    /* parse prompt */
                    strcpy(tmp_task.task_name,token);
                    break;
                case 1:
                    ;
                    /* parse answer */
                    strcpy(tmp_task.task_text,token);
                    break;
                case 2:
                    ;
                    /* parse answer */
                    strcpy(tmp_task.date,token);
                    break;
                default:
                    ;
                    continue;
                    ;;
            }
            ++flag;
            token = strtok(NULL, ",");
        }
        /* append to task array  */
        if (lines>=size_of_task_array) {
            size_of_task_array*=2;
            task_array = realloc(task_array,size_of_task_array*sizeof(struct Task));
        }
        task_array[lines] = tmp_task;
        ++lines;
    }
    *p_amount_of_tasks = lines;
    return task_array;
}

void print_task(struct Task task) {
	printf("task name: %s, task text: %s, task date: %s\n",task.task_name,task.task_text,task.date);
}


int remove_task_by_name(char *filepath, char* task_to_remove) {
	  /* Open original file for reading and a temp file for writing*/
	  FILE *file, *temp;
	  char buffer[SIZE];
	  char mod_buffer[SIZE];
	  char* temp_filename = "tmp_file,csv";
	  file = fopen(filepath, "r");
	  temp = fopen(temp_filename, "w");

	  if (file == NULL || temp == NULL)
	  {
	    printf("Error opening files(s).\n");
	    return 1;
	  }

	  bool keep_reading = true;

	  int current_line = 1;

	  do
	  {
	    fgets(buffer, SIZE, file);
        strcpy(mod_buffer,buffer);
        char* task_name = strtok(mod_buffer,",");
        if (!task_name) {
            task_name = "";
        }
        printf("\nToken name when scanning for removal: %s\n",task_name);

	    /* Check for end of file */
	    if (feof(file)) {
		    keep_reading = false;
	    }
	    /* Replace if first column is the name to remove */
	    else if (!strcmp(task_name,task_to_remove)) {
            printf("\nFound a match: %s and %s\n",task_name,task_to_remove);
            current_line++;
	    }
	    else  {
	    fputs(buffer, temp);
	    current_line++;
	    }

	  } while (keep_reading);

	  fclose(file);
	  fclose(temp);

	  remove(filepath);
	  rename(temp_filename, filepath);
	  return 1;
}


int add_task(char* task_name, char* task_text, char* date,char* task_path) {
    FILE *task_list = NULL;
    task_list = fopen(task_path,"a");
    if (!task_list) {
        fprintf(stderr,"Couldn't open file %s\n",task_path);
        return -1;
    }
    fprintf(task_list,"%s,%s,%s\n",task_name,task_text,task_path);
    fclose(task_list);
    return 1;
};


int main(int argc, char** argv) {
	char task_path[1024];
    char * home = getenv("HOME");
    int start_of_loop = 0;
	/* Set up task_path so it points to the flashcards folder */
	sprintf(task_path,"%s/%s",home,PATH_TO_FILE_FROM_HOME);
    printf("\nInitial set up complete\n");
    add_task("added task in code", "hope this text is inputted correctly", "2021-04-11", task_path);
    remove_task_by_name(task_path, "\"new_name\"");
    //remove_task_by_name(task_path, "new_name");

    struct Task* task_array;
    int amount_of_tasks;
    printf("\nReading files... %s\n",task_path);
    task_array = csv_to_task_array(task_path,&amount_of_tasks);
    printf("\nReaded files...\n");


    if (strcmp("task_name",task_array[0].task_name) == 0) 
        start_of_loop = 1;
    for (int i = start_of_loop; i<amount_of_tasks; i++) {
        struct Task current_task = task_array[i];
        print_task(current_task);
    }


    return EXIT_SUCCESS;
}
