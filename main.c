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

#define BLK "\e[0;30m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define BLU "\e[0;34m"
#define MAG "\e[0;35m"
#define CYN "\e[0;36m"
#define WHT "\e[0;37m"

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
        struct Task tmp_task = {};
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

	    /* Check for end of file */
	    if (feof(file)) {
		    keep_reading = false;
	    }
	    /* Replace if first column is the name to remove */
	    else if (!strcmp(task_name,task_to_remove)) {
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
    fprintf(task_list,"%s,%s,%s\n",task_name,task_text,date);
    fclose(task_list);
    return 1;
};


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
    size_t ret = strftime(current_day, sizeof(current_day), "%F", tm);

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
};

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
    size_t ret = strftime(current_day, sizeof(current_day), "%F", tm);


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

};

void display_upcoming(char* task_path) {
    struct Task* task_array;
    int amount_of_tasks;
    task_array = csv_to_task_array(task_path,&amount_of_tasks);
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char current_day[64];
    size_t ret = strftime(current_day, sizeof(current_day), "%F", tm);


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


};

char help[] = "\nSherlly's Better Task Manager \n--help\t-h\t\tHelp \n--upcoming\t-u\tPrints status bar summary |❌8 ❗5  📅0 | \n--list\t-l\t\tLists tasks with colour coded urgency \n--add\t-a\t\tAdd task like so btm --add -n \"name\" -t \"task desc\" -d \"YYYY-mm-dd\" \n--remove\t-r\t\tRemove task like so btm --remove \"name\"\n\n";

int main(int argc, char** argv) {
	char task_path[1024];
    char * home = getenv("HOME");
	/* Set up task_path so it points to the flashcards folder */
	sprintf(task_path,"%s/%s",home,PATH_TO_FILE_FROM_HOME);
    if (!strcmp(argv[1],"--help") || !strcmp(argv[1],"-h")) {
        printf("%s\n",help);
    }
    else if (!strcmp(argv[1],"--upcoming") || !strcmp(argv[1],"-u")) {
        display_upcoming(task_path);
    }
    else if (!strcmp(argv[1],"--list") || !strcmp(argv[1],"-l")) {
        display_tasks(task_path);
    }
    else if (!strcmp(argv[1],"--add") || !strcmp(argv[1],"-a")) {
       char* name; 
       char* text; 
       char* date; 
       for (int c = 2; c<argc;c++) {
           char* current_arg = argv[c];
           if (!strcmp(current_arg,"-n") || !strcmp(current_arg,"--name")) {
               name = argv[++c];
           }
           else if (!strcmp(current_arg,"-t") || !strcmp(current_arg,"--text")) {
               text = argv[++c];
           }
           else if (!strcmp(current_arg,"-d") || !strcmp(current_arg,"--date")) {
               date = argv[++c];
           }
       }
       add_task(name,text,date,task_path);

    }
    else if (!strcmp(argv[1],"--remove") || !strcmp(argv[1],"-r")) {
        remove_task_by_name(task_path,argv[2]);
    }
    else {
        fprintf(stderr,"Unable to parse arguments, see help");
        printf("%s\n",help);
        exit(0);
    }

    return EXIT_SUCCESS;
}