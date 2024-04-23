#include <string.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct {
    int d, m, y;
} Date ;

void print_date(Date dt) {
    printf("%d-%d-%d\n",dt.y,dt.m,dt.d);
}
  


const int monthDays[12]
    = { 31, 28, 31, 30, 31, 30, 
       31, 31, 30, 31, 30, 31 };
  


int countLeapYears(Date d)
{
    int years = d.y;
  
    
    
    
    if (d.m <= 2)
        years--;
  
    
    
    
     
    return years / 4 
           - years / 100
           + years / 400;
}
  
int getDifference(Date dt1, Date dt2)
{
  
    long int n1 = dt1.y * 365 + dt1.d;
  
    for (int i = 0; i < dt1.m - 1; i++)
        n1 += monthDays[i];
  
    
    
    n1 += countLeapYears(dt1);
  
    
    
  
    long int n2 = dt2.y * 365 + dt2.d;
    for (int i = 0; i < dt2.m - 1; i++)
        n2 += monthDays[i];
    n2 += countLeapYears(dt2);
  
    
    return (n1 - n2);
} 

void parse_date(char* date_string,Date *date_buffer) {
    char* token;
    int top = -1;
    int array[10];
    token = strtok(date_string, "-");
    while( token != NULL )
    {
        top++;
        array[top] = atoi(token);

        token = strtok(NULL, "-");
    }

    date_buffer->d = array[2];
    date_buffer->m = array[1];
    date_buffer->y = array[0];
}

int getDifference_str(char* date_1,char* date_2)
{
    Date dt1;
    Date dt2;
    /* check if required */
    char date_1_buff[60];
    char date_2_buff[60];
    strcpy(date_1_buff,date_1);
    strcpy(date_2_buff,date_2);
    parse_date(date_1_buff, &dt1);
    parse_date(date_2_buff, &dt2);
    
    
  
    
    long int n1 = dt1.y * 365 + dt1.d;
  
    
    for (int i = 0; i < dt1.m - 1; i++)
        n1 += monthDays[i];
  
    
    
    n1 += countLeapYears(dt1);
  
    
    
  
    long int n2 = dt2.y * 365 + dt2.d;
    for (int i = 0; i < dt2.m - 1; i++)
        n2 += monthDays[i];
    n2 += countLeapYears(dt2);
  
    
    return (n1 - n2);
} 
