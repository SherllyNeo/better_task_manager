#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "dates.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>
#include "config.h"
#include "dsvParser.h"


#define RED     "\x1b[31m"
#define GRN   "\x1b[32m"
#define YEL   "\x1b[0;33m"
#define ANSI_COLOR_RESET   "\x1b[0m"


#define SIZE 10000

typedef enum {
    LOW,
    HIGH,
    URGENT
} Emergency;

typedef struct {
    char date[SIZE];
    char task_name[SIZE];
    char task_text[SIZE];
} Task;

Emergency categorise_task_date(char* task_date) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char current_day[64];
    strftime(current_day, sizeof(current_day), "%F", tm);
    

    int days_to_task = getDifference_str(task_date,current_day);
    Emergency priority;
    if (days_to_task < URGENT_TOLERENCE ) {
        priority = URGENT;
    }
    else if (days_to_task < HIGH_PRIORITY_TOLERENCE) {
        priority = HIGH;
    }
    else  {
        priority = LOW;
    }
    return priority;
}


void print_task(Task task,char* current_day) {
    int days_to_task = getDifference_str(task.date,current_day);
    Emergency priority = categorise_task_date(task.date);
    if (priority == URGENT) {
	    printf(RED "\n------------\ntask name: %s \ntask text: %s \ntask date: %s \ndays to task: %d \n------------\n",task.task_name,task.task_text,task.date,days_to_task);
    }
    else if (priority == HIGH) {
	    printf(YEL "\n------------\ntask name: %s \ntask text: %s \ntask date: %s \ndays to task: %d \n------------\n",task.task_name,task.task_text,task.date,days_to_task);
    }
    else {
	    printf(GRN "\n------------\ntask name: %s \ntask text: %s \ntask date: %s \ndays to task: %d \n------------\n",task.task_name,task.task_text,task.date,days_to_task);
    }
}

Task* csv_to_task_array(char* task_deck,int* p_amount_of_tasks) {
    if (task_deck == NULL) {
        fprintf(stderr, "file path is null\n");
        return NULL;
    }

    DSV parsed_csv = dsvParseFile(task_deck, ',');
    if (!parsed_csv.valid) {
        fprintf(stderr, "unable to parse csv\n");
        return NULL;
    }


    *p_amount_of_tasks = (int)parsed_csv.rows;


    Task *task_array = calloc(parsed_csv.rows,sizeof(Task));

    for (size_t row = 0; row<parsed_csv.rows;row++) {
        strncpy(task_array[row].task_name,parsed_csv.content[row][0],SIZE);
        strncpy(task_array[row].task_text,parsed_csv.content[row][1],SIZE);
        strncpy(task_array[row].date,parsed_csv.content[row][2],SIZE);

        if (strlen(task_array[row].date) <= 0) {
            strncpy(task_array[row].date," \0",2);
        }
        if (strlen(task_array[row].task_name) <= 0) {
            strncpy(task_array[row].task_name," \0",2);
        }
        if (strlen(task_array[row].task_text) <= 0) {
            strncpy(task_array[row].task_text," \0",2); 
        }
    }

    int freed = dsvFreeDSV(parsed_csv);
    if (freed) {
        fprintf(stderr, "unable to free parsed csv\n");
    }


    return task_array;

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
	    return 0;
	  }

      bool removed = false;
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

	    /* Check for end of file */
	    if (feof(file)) {
		    keep_reading = false;
	    }
	    /* Replace if first column is the name to remove */
	    else if (!strcmp(task_name,task_to_remove)) {
            current_line++;
            removed = true;
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
      if (!removed) {
          fprintf(stderr,"Task name %s was not found\n",task_to_remove);
          return 0;
      };
	  return 1;
}


int add_task(char* task_name, char* task_text, char* date,char* task_path) {
    FILE *task_list = NULL;
    task_list = fopen(task_path,"a");
    if (!task_list) {
        fprintf(stderr,"Couldn't open file %s\n",task_path);
        return 0;
    }
    fprintf(task_list,"%s,%s,%s\n",task_name,task_text,date);
    fclose(task_list);
    return 1;
}


void swap(Task* xp, Task* yp)
{
    Task temp = *xp;
    *xp = *yp;
    *yp = temp;
}
  
void selectionSort(Task* arr,int size_of_array)
{
    int i, j, min_idx;
  
    for (i = 0; i < size_of_array - 1; i++) {
        min_idx = i;
        for (j = i + 1; j < size_of_array; j++) {
            int difference = getDifference_str(arr[j].date,arr[min_idx].date);
            if (difference < 0) 
                min_idx = j;
            }
        swap(&arr[min_idx], &arr[i]);
    }
}


void display_tasks(char* task_path) {
    int amount_of_tasks = 0;
    Task* task_array = csv_to_task_array(task_path,&amount_of_tasks);
    if (!task_array) {
        fprintf(stderr, "unable to read task array\n");
        exit(EXIT_FAILURE);
    }


    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char current_day[64];
    strftime(current_day, sizeof(current_day), "%F", tm);

    if (strcmp("task_name",task_array[0].task_name) == 0) {
        for (int j = 0; j<amount_of_tasks; j++) {
            task_array[j] = task_array[j+1];
        }
        amount_of_tasks--;
    }

    selectionSort(task_array,amount_of_tasks);
    


    for (int i = 0; i<amount_of_tasks; i++) {
        Task current_task = task_array[i];
        print_task(current_task,current_day);
    }

    free(task_array);

}

void display_upcoming(char* task_path) {
    Task* task_array;
    int amount_of_tasks;
    task_array = csv_to_task_array(task_path,&amount_of_tasks);
    if (strcmp("task_name",task_array[0].task_name) == 0) {
        for (int j = 0; j<amount_of_tasks; j++) {
            task_array[j] = task_array[j+1];
        }
        amount_of_tasks--;
    }

    selectionSort(task_array,amount_of_tasks);

    int count_of_urgent = 0;
    int count_of_high = 0;
    int count_of_low = 0;
    
    for (int i = 0; i<amount_of_tasks; i++) {
        Task current_task = task_array[i];
        Emergency category = categorise_task_date(current_task.date);
        if (category == URGENT) {
            count_of_urgent++;
        }
        else if (category == HIGH) {
            count_of_high++;
        }
        else {
            count_of_low++;
        }
    }

    printf("|❌%d ❗%d  📅%d |\n",count_of_urgent,count_of_high,count_of_low);


}

