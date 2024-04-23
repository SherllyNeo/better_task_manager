#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>
#include "task_manager_file.h"


char help[] = "\nSherlly's Better Task Manager \n--help\t-h\t\tHelp \n--upcoming\t-u\tPrints status bar summary |❌8 ❗5  📅0 | \n--list\t-l\t\tLists tasks with colour coded urgency \n--add\t-a\t\tAdd task like so btm --add -n \"name\" -t \"task desc\" -d \"YYYY-mm-dd\" \n--remove\t-r\t\tRemove task like so btm --remove \"name\"\n\n";

int main(int argc, char** argv) {
    if (argc<2) {
        fprintf(stderr,"Must have at least two arguements"); 
        printf("%s\n",help);
        exit(EXIT_FAILURE);
    };
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
       int check_if_added = add_task(name,text,date,task_path);
       if (check_if_added) {
           printf("\nTask added!\n");
       }
       else{
           fprintf(stderr,"\nError adding task!\n");
       }

    }
    else if (!strcmp(argv[1],"--remove") || !strcmp(argv[1],"-r")) {
        int check_if_removed = remove_task_by_name(task_path,argv[2]);
        if (check_if_removed) {
            printf("\nTask removed!\n");
        }
        else{
            fprintf(stderr,"Error removing task!\n");
        }
    }
    else {
        fprintf(stderr,"Unable to parse arguments, see help");
        printf("%s\n",help);
        exit(0);
    }

    return EXIT_SUCCESS;
}
