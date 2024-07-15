#include "myshell.h"

int main() 
{
    char cmdline[MAXLINE]; /* Command line */

    while (1) {
	/* Read */
	printf("CSE4100-SP-P2> ");                   
	fgets(cmdline, MAXLINE, stdin);
	if (feof(stdin))
	    exit(0);

	/* Evaluate */
	eval(cmdline);
    } 
}
/* $end shellmain */
  
/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline) 
{
    char *argv[MAXARGS]; /* Argument list execve() */
    char buf[MAXLINE];   /* Holds modified command line */
    int bg;              /* Should the job run in bg or fg? */
    pid_t pid;           /* Process id */
    
    strcpy(buf, cmdline);
    bg = parseline(buf, argv); 
    if (argv[0] == NULL)  
	return;   /* Ignore empty lines */

    char add_cmdline[200] = "/bin/";
    if (!strcmp(argv[0], "sort")) strcpy(add_cmdline, "/usr/bin/");
    else strcpy(add_cmdline, "/bin/");
    strcat(add_cmdline, argv[0]);
    if (!builtin_command(argv)) { //quit -> exit(0), & -> ignore, other -> run
        if ((pid = Fork()) == 0) {
            if (execve(add_cmdline, argv, environ) < 0) {	//ex) /bin/ls ls -al &
                printf("%s: Command not found.\n", argv[0]);
                exit(0);
            }
        }
    	/* Parent waits for foreground job to terminate */
        else {
            if (!bg){ 
	        int status;
            waitpid(pid, &status, 0);
	        }
	        else//when there is backgrount process!
	            printf("%d %s", pid, cmdline);
        }
        
    }
	
    return;
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv) 
{
    if (!strcmp(argv[0], "exit"))
    exit(0);
    if (!strcmp(argv[0], "quit")) /* quit command */
	exit(0);  
    if (!strcmp(argv[0], "&"))    /* Ignore singleton & */
	return 1;
    if (!strcmp(argv[0], "cd")){
        if (argv[1] == NULL || !strcmp(argv[1], "~")) {
            chdir(getenv("HOME"));
            return 1;
        }
        else if (!strcmp(argv[1], ".")) return 1;
        else {
            chdir(argv[1]);
            return 1;
        }
    }
    return 0;                     /* Not a builtin command */
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv) 
{
    char *delim;         /* Points to first space delimiter */
    int argc;            /* Number of args */
    int bg;              /* Background job? */

    buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* Ignore leading spaces */
	buf++;

    /* Build the argv list */
    argc = 0;
    while ((delim = strchr(buf, ' '))) {
	argv[argc++] = buf;
	*delim = '\0';
	buf = delim + 1;
	while (*buf && (*buf == ' ')) /* Ignore spaces */
            buf++;
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* Ignore blank line */
	return 1;

    /* Should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0)
	argv[--argc] = NULL;

    for (int i = 0 ; i < argc; i++) {
        char *word = argv[i];
        if (word[0] == '"' && word[strlen(word) -1] == '"') {
            for (int j = 0 ; j < strlen(word)-1; j++) {
                word[j] = word[j+1];
            }
            word[strlen(word)-2] = '\0';
            argv[i] = word;
        }
    }
    
    return bg;
}
/* $end parseline */


