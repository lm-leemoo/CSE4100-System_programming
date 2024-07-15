/* $begin shellmain */
#include "csapp.h"
#include <errno.h>
#define MAXARGS   128

/* Function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv); 
extern int pid_num;
extern int stop_flag;
extern int pid;
extern int forpid_num;