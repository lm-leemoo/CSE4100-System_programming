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
    pid_t pid2;
    int pipe_num = 0; 
    char *new_buf[MAXARGS];
    strcpy(buf, cmdline);

    char *token = strtok(buf, "|");
    while(token != NULL) {
        new_buf[pipe_num] = (char*)malloc(sizeof(char)*MAXLINE);
        strcpy(new_buf[pipe_num], token);
        pipe_num++;
        token = strtok(NULL, "|");
    }//pipe 개수를 세기
    pipe_num--;

    if (pipe_num == 0){ //파이프 없이 명령어 한 개일 때
        bg = parseline(buf, argv); //argv로 잘라서 저장
        if (argv[0] == NULL) return;   /* Ignore empty lines */

        if (!builtin_command(argv)) { //quit -> exit(0), & -> ignore, other -> run
            char add_cmdline[200];
            if (!strcmp(argv[0], "sort")) strcpy(add_cmdline, "/usr/bin/");
            else strcpy(add_cmdline, "/bin/");
            strcat(add_cmdline, argv[0]); //argv[0]에 bin을 추가해서 실행
            if ((pid = Fork()) == 0) {
                if (execve(add_cmdline, argv, environ) < 0) {	//ex) /bin/ls ls -al &
                    printf("%s: Command not found.\n", argv[0]);   
                }
                exit(0);
            }

    	/* Parent waits for foreground job to terminate */
            else {
                if (!bg){ 
	                int status;
                    waitpid(pid, &status, 0);
	            }
	            else{//when there is backgrount process!
                    printf("%d %s", pid, cmdline);
                }
            }
        
        }
    }

    else { // 명령어 두 개 이상일 때
        int fds[pipe_num+1][2];
        int pid[pipe_num+1];
        for (int i = 0 ; i <= pipe_num ; i++) {
            if (pipe(fds[i])<0) printf("error in pipe\n");
            bg = parseline(new_buf[i], argv);
            if (argv[0]==NULL) return;
            if (!builtin_command(argv)) {
                char add_cmdline[200];
                if (!strcmp(argv[0], "sort")) strcpy(add_cmdline, "/usr/bin/");
                else strcpy(add_cmdline, "/bin/");
                strcat(add_cmdline, argv[0]);
                if ((pid[i]=Fork())==0) {//child
                    if (i == 0) { //first
                        close(fds[i][0]);
                        dup2(fds[i][1], STDOUT_FILENO);
                        close(fds[i][1]);                 
                    }

                    else if (i == pipe_num){//last
                        close(fds[i-1][1]);
                        dup2(fds[i-1][0], STDIN_FILENO);
                        close(fds[i-1][0]);     
                    }

                    else {
                        close(fds[i-1][1]);
                        dup2(fds[i-1][0], STDIN_FILENO);
                        close(fds[i-1][0]);
                        close(fds[i][0]);
                        dup2(fds[i][1], STDOUT_FILENO);
                        close(fds[i][1]);
                    }

                    if (execve(add_cmdline, argv, environ) < 0) {	//ex) /bin/ls ls -al &
                            printf("%s: Command not found.\n", argv[0]);
                            exit(0);
                    }
                    //exit(0);
                }
            }
            if (i >0) {
                 close(fds[i-1][0]);
                 close(fds[i-1][1]);
            }
            if (waitpid(pid[i], NULL, 0)<0) unix_error("wow\n");        
        }
        close(fds[pipe_num][0]);
        close(fds[pipe_num][1]);
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


