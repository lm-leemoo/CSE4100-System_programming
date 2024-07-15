/* 
 * echoserveri.c - An iterative echo server 
 */ 
/* $begin echoserverimain */
#include "csapp.h"
#include <pthread.h>
#define SBUFSIZE 16
#define NTHREADS 100

void echo(int connfd);
static int byte_cnt = 0;
char response[100000000];
static sem_t mutex;

typedef struct item{
    int ID;
    int left_stock;
    int price;
    int readcnt;
    sem_t mutex;
}item;

typedef struct node{
    item contents;
    struct node* left;
    struct node* right;
}node;
node *root = NULL;

typedef struct {
    int *buf;           /* Buffer array */
    int n;              /* Maximum number of slots */
    int front;          /* buf[(front+1)%n] is first item */
    int rear;           /* buf[rear%n] is last item */
    sem_t mutex;        /* Protects accesses to buf */
    sem_t slots;        /* Counts available slots */
    sem_t items;        /* Counts available items */
} sbuf_t;
sbuf_t sbuf;

void store_txt(FILE*fp, node *cur)
{
    if (cur == NULL) {
        return;
    }
    fprintf(fp, "%d %d %d\n", cur->contents.ID, cur->contents.left_stock, cur->contents.price);
    store_txt(fp, cur -> left);
    store_txt(fp, cur -> right);
}

void init_tree()
{
    FILE *fp = fopen("stock.txt", "r");
    item newitem;
    while(1){
        if (fscanf(fp, "%d %d %d", &newitem.ID, &newitem.left_stock, &newitem.price) ==EOF) break;
        Sem_init(&newitem.mutex, 0, 1);
        node* newnode = (node*)malloc(sizeof(node));
        newnode -> contents = newitem;
        newnode -> left = NULL;
        newnode -> right = NULL;
        
        if (root == NULL) root = newnode; //첫번쨰
        else{
            node *search = root;
            while (1){
                if (search->contents.ID < newnode->contents.ID) {
                    if (search->right == NULL) {
                        search->right = newnode;
                        break;
                    }
                    search = search -> right;
                }
                else {
                    if (search->left == NULL) {
                        search->left = newnode;
                        break;
                    }
                    search = search -> left;
                }
            }
        }

    }
    fclose(fp);
}

void sbuf_init(sbuf_t *sp, int n)
{
    sp->buf = Calloc(n, sizeof(int));
    sp->n = n;                  /* Buffer holds max of n items */
    sp->front = sp->rear = 0;   /* Empty buffer iff front == rear */
    Sem_init(&sp->mutex, 0, 1); /* Binary semaphore for locking */
    Sem_init(&sp->slots, 0, n); /* Initially, buf has n empty slots */
    Sem_init(&sp->items, 0, 0); /* Initially, buf has 0 items */
}

void sbuf_insert(sbuf_t *sp, int item)
{
    P(&sp->slots);
    P(&sp->mutex);
    sp->buf[(++sp->rear)%(sp->n)] = item;
    V(&sp->mutex);
    V(&sp->items);
}

int sbuf_remove(sbuf_t *sp)
{
    int item;
    P(&sp->items);
    P(&sp->mutex);
    item = sp->buf[(++sp->front)%(sp->n)];
    V(&sp->mutex);
    V(&sp->slots);
    return item;
}

void showcommand(node* cur)
{
    if (cur == NULL) return;
    showcommand(cur->left);
    char line[50];
    sprintf(line, "%d %d %d\n", (cur->contents).ID, (cur->contents).left_stock, (cur->contents).price);
    strcat(response, line);
    showcommand(cur->right);
}

void buycommand(int ID, int num)
{
    node *search = root;
    char errorline[50] = "ID error\n";
    strcpy(response, errorline);
    while(1){
        if (search == NULL) return;
        if (search->contents.ID < ID) search = search -> right;
        else if (search->contents.ID > ID) search = search -> left;
        else if (search -> contents. ID == ID) break;
        else return;
    }
    if (search -> contents.left_stock < num) {
        char line[50] = "Not enougth left stock\n";
        strcpy(response, line);
        return;
    }
    P(&search->contents.mutex);
    search->contents.left_stock -= num;
    char line[50] = "[buy] success\n";
    strcpy(response, line);
    V(&search->contents.mutex);
}

void sellcommand(int ID, int num)
{
    node *search = root;
    char line[50] = "[sell] success\n";
    strcpy(response, line);
    while (1) {
        if (search == NULL) return;
        if (search->contents.ID < ID) search = search -> right;
        else if (search->contents.ID > ID) search = search -> left;
        else if (search->contents.ID == ID) break;
        else return;
    }
    P(&search->contents.mutex);
    search->contents.left_stock += num;
    V(&search->contents.mutex);
}

static void init_echo_cnt(void)
{
    Sem_init(&mutex, 0, 1);
    byte_cnt= 0;
}

void echo_cnt(int connfd)
{
    int n;
    char buf[MAXLINE];
    rio_t rio;
    static  pthread_once_t once = PTHREAD_ONCE_INIT;
    Pthread_once(&once, init_echo_cnt); 
    Rio_readinitb(&rio, connfd);        
    while ( ( n = Rio_readlineb(&rio, buf, MAXLINE) ) != 0 ) {
        P(&mutex);
        byte_cnt += n;
        printf("Server received %d (%d total) bytes\n", n, byte_cnt); 
        V(&mutex);
        memset(response, 0, sizeof(response));
        if (!strcmp(buf, "show\n")){ //show
            showcommand(root);
        }
        else if (!strcmp(buf, "exit\n")){ //exit
            char errorline[10] ="exit\n";
            strcpy(response, errorline);
            return;
        }
        else {
            char command[10] = "";
            int ID = 0;
            int num = 0;
            sscanf(buf, "%s %d %d\n", command, &ID, &num);
            if (!strcmp(command, "buy")) buycommand(ID, num); //buy
            else if(!strcmp(command, "sell")) sellcommand(ID, num); //sell
            else {
                char errorline[50] = "comamnd error\n";
                strcpy(response, errorline);
            }
        }

        Rio_writen(connfd, response, MAXLINE);
        FILE *fp = fopen("stock.txt", "w");
        store_txt(fp, root);
        fclose(fp);
    }
}

void *thread(void *vargp) {
    Pthread_detach(pthread_self());
    while ( 1 ) {
        int connfd = sbuf_remove(&sbuf); /* Remove connfd from buf */
        echo_cnt(connfd);                /* Service client */
        Close(connfd);
    }
}

int main(int argc, char **argv) 
{
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;  /* Enough space for any address */  //line:netp:echoserveri:sockaddrstorage
    char client_hostname[MAXLINE], client_port[MAXLINE];
    pthread_t tid;

    if (argc != 2) {
	    fprintf(stderr, "usage: %s <port>\n", argv[0]);
	    exit(0);
    }

    init_tree(root);
    listenfd = Open_listenfd(argv[1]);
    
    sbuf_init(&sbuf, SBUFSIZE);
    for (int i = 0; i < NTHREADS; i++)
        Pthread_create(&tid, NULL, thread, NULL);

    while (1) {
        clientlen = sizeof(struct sockaddr_storage);
	    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        sbuf_insert(&sbuf, connfd);
        Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
        printf("Connected to (%s, %s)\n", client_hostname, client_port);
    }
    exit(0);
}
 

