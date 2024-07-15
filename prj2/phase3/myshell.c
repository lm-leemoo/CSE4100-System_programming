#include "myshell.h"
#include <errno.h>
int forpid_num = 1;
int pid_num = 1;
int stop_flag = 1;
int pid;
int pid_arr[MAXARGS] ={0,};
char com_arr[MAXARGS][MAXLINE] = {{0},};
char state_arr[MAXARGS][MAXLINE] = {{0}, };
int forpid_arr[MAXARGS] ={0,};
char forcom_arr[MAXARGS][MAXLINE] = {{0},};
char forstate_arr[MAXARGS][MAXLINE] = {{0}, };

void sigchild_handler(int sig)
{	
	int temp_errno = errno;
	sigset_t mask, prev;
    pid_t pid;
    sigfillset(&mask);

    while((pid=waitpid(-1,NULL, WNOHANG))>0)
    {
        sigprocmask(0,&mask,&prev);
        for(int i=1;i<pid_num;i++)
        {
            if(pid_arr[i] == pid)
            {
                pid_arr[i] = -5;
                break;	
            }
        }
        sigprocmask(SIG_BLOCK,&prev,NULL);
        
    }
	errno = temp_errno;
}

void sigtstp_handler(int sig)
{	
	int temp_errno = errno;
	sigset_t mask, prev;
    pid_t pid;
    sigfillset(&mask);
    sigprocmask(0,&mask,&prev);

    for(int i=1;i<forpid_num;i++) {
        Kill(forpid_arr[i], SIGTSTP);
        forpid_arr[i] = -5;
    }

    sigprocmask(SIG_BLOCK,&prev,NULL);
	errno = temp_errno;
}

int main() 
{
    Signal(SIGCHLD, sigchild_handler);
    Signal(SIGTSTP, sigtstp_handler);
    char cmdline[MAXLINE]; /* Command line */
    while (1) {
	/* Read */
    sleep(1);
	printf("CSE4100-SP-P2> ");
    fflush(stdout);
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
    sigset_t mask, prev;
    sigfillset(&mask);
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
            char add_cmdline[200] = {0};
            if (!strcmp(argv[0], "sort")) strcpy(add_cmdline, "/usr/bin/");
            else if (argv[0][0] == '.') ;
            else strcpy(add_cmdline, "/bin/");
            strcat(add_cmdline, argv[0]); //argv[0]에 bin을 추가해서 실행
            if ((pid = Fork()) == 0){
                if (execvp(add_cmdline, argv) < 0) {	//ex) /bin/ls ls -al &
                    printf("%s: Command not found.\n", argv[0]);   
                }
                exit(0);
            }

    	/* Parent waits for foreground job to terminate */
            else {
                if (!bg){ 
	                forpid_arr[forpid_num] = pid;
                    strcpy(forcom_arr[forpid_num], cmdline);
                    strcpy(forstate_arr[forpid_num], cmdline);
                    forpid_num++;
	            }
	            else{//when there is backgrount process!
                    pid_arr[pid_num] = pid;
                    strcpy(com_arr[pid_num], cmdline);
                    char *state = "running";
                    strcpy(state_arr[pid_num], state);
                    printf("[%d] %d\n", pid_num, pid_arr[pid_num]);
                    pid_num++;
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
                char add_cmdline[200] = {0};
                if (!strcmp(argv[0], "sort")) strcpy(add_cmdline, "/usr/bin/");
                else if (argv[0][0] == '.') ;
                else strcpy(add_cmdline, "/bin/");
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

                    if (execvp(add_cmdline, argv) < 0) {	//ex) /bin/ls ls -al &
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

    if (!strcmp(argv[0], "jobs"))
    {
        
        for (int i = 1; i < pid_num; i++) {
            if (pid_arr[i] == -5){//종료된 애
                for (int j = i ; j < pid_num; j++) {
                    pid_arr[j] = pid_arr[j+1];
                    strcpy(state_arr[j], state_arr[j+1]);
                    strcpy(com_arr[j], com_arr[j+1]);
                }
                pid_num--;
                i--;
            }
        }
        for (int i = 1; i < forpid_num; i++) {
            if (forpid_arr[i] == -5){//stopped된 foreground
                pid_arr[pid_num] = forpid_arr[i];
                char *state = "stopped";
                strcpy(state_arr[pid_num], state);
                strcpy(com_arr[pid_num], forcom_arr[i]);
                pid_num++;
            }
        }
        for (int i = 1; i < forpid_num; i++) {
            if (forpid_arr[i] == -5){//stopped된 foreground
                for (int j = i; j < forpid_num; j++) {
                    forpid_arr[j] = forpid_arr[j+1];
                    strcpy(forstate_arr[j], forstate_arr[j+1]);
                    strcpy(forcom_arr[j], forcom_arr[j+1]);
                }
                forpid_num--;
                i--;
            }
        }

        for(int i=1; i<pid_num;i++)
        {
            printf("[%d] %s %s\n",i, state_arr[i], com_arr[i]);
        }
       return 1;
    }

    if (!strcmp(argv[0], "kill")){
        if(argv[1][0] == '%'){
            int number = 0;
            number= atoi(argv[1]+1);
            Kill(pid_arr[number], SIGKILL);
            for (int j = number ; j < pid_num; j++) {
                pid_arr[j] = pid_arr[j+1];
                strcpy(state_arr[j], state_arr[j+1]);
                strcpy(com_arr[j], com_arr[j+1]);
            }
            pid_num--;
        }
        return 1;
    }
    if (!strcmp(argv[0], "bg")){
        int number = 0;
        if(argv[1][0] == '%'){
            number= atoi(argv[1]+1);
            Kill(pid_arr[number], SIGCONT);
            char *state = "running";
            strcpy(state_arr[number], state);
        }
        return 1;
    }

    if (!strcmp(argv[0], "fg")){
        int number = 0;
        if(argv[1][0] == '%'){
            number= atoi(argv[1]+1);
            Kill(pid_arr[number], SIGCONT);
            int status;
            for (int j = number ; j < pid_num; j++) {
                pid_arr[j] = pid_arr[j+1];
                strcpy(state_arr[j], state_arr[j+1]);
                strcpy(com_arr[j], com_arr[j+1]);
            }
            pid_num--;
        }
        return 1;
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
    if ((bg = (buf[strlen(buf)-2] == '&')) != 0)
    buf[strlen(buf)-2] = '\0';
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


