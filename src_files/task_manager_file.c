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


#define RED     "\x1b[31m"
#define GRN   "\x1b[32m"
#define YEL   "\x1b[0;33m"
#define ANSI_COLOR_RESET   "\x1b[0m"


#define SIZE 10000

enum Emergency {
    LOW,
    HIGH,
    URGENT
};

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
        struct Task tmp_task = {'\0'};
        while (token) {
            /* this will be 3 each time. The promt, answer, date and delay */
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


void swap(struct Task* xp, struct Task* yp)
{
    struct Task temp = *xp;
    *xp = *yp;
    *yp = temp;
}
  
void selectionSort(struct Task* arr,int size_of_array)
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

enum Emergency categorise_task_date(char* task_date) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char current_day[64];
    strftime(current_day, sizeof(current_day), "%F", tm);
    

    int days_to_task = getDifference_str(task_date,current_day);
    enum Emergency priority;
    if (days_to_task < 0 ) {
        priority = URGENT;
    }
    else if (days_to_task < 2) {
        priority = HIGH;
    }
    else  {
        priority = LOW;
    }
    return priority;
}

void print_task(struct Task task,char* current_day) {
    int days_to_task = getDifference_str(task.date,current_day);
    enum Emergency priority = categorise_task_date(task.date);
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

void display_tasks(char* task_path) {
    struct Task* task_array;
    int amount_of_tasks;
    task_array = csv_to_task_array(task_path,&amount_of_tasks);
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
        struct Task current_task = task_array[i];
        print_task(current_task,current_day);
    }

}

void display_upcoming(char* task_path) {
    struct Task* task_array;
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
        struct Task current_task = task_array[i];
        enum Emergency category = categorise_task_date(current_task.date);
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

