/* 
 * echoserveri.c - An iterative echo server 
 */ 
/* $begin echoserverimain */
#include "csapp.h"
void echo(int connfd);
int byte_cnt = 0;
char response[100000];

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
    int maxfd;
    fd_set read_set;
    fd_set ready_set;
    int nready;
    int maxi;
    int clientfd[FD_SETSIZE];
    rio_t clientrio[FD_SETSIZE];
}pool;

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

void init_pool(int listenfd, pool *p)
{
    /* Initially, there are no connected descriptors*/
    int i;
    p->maxi = -1;
    for(i = 0; i < FD_SETSIZE; i++) p->clientfd[i] = -1;
    p->maxfd = listenfd;
    FD_ZERO(&p->read_set);
    FD_SET(listenfd, &p->read_set);
}

void add_client(int connfd, pool *p)
{
    int i;
    p->nready--;
    for(i = 0; i < FD_SETSIZE; i++){
        if(p->clientfd[i] < 0){
            p->clientfd[i] = connfd;
            Rio_readinitb(&p->clientrio[i], connfd);
            
            /*Add the descriptor to descriptor set*/
            FD_SET(connfd, &p->read_set);

            /*Update max descriptor and pool high water mark*/
            if(connfd > p->maxfd) p->maxfd = connfd;
            if(i > p->maxi) p->maxi = i;
            break;
        }
    }
    if(i == FD_SETSIZE) app_error("add_client error: Too many clients");
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
    search->contents.left_stock -= num;
    char line[50] = "[buy] success\n";
    strcpy(response, line);
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
    search->contents.left_stock += num;
}


void check_clients(pool *p){
    int i, connfd, n;
    char buf[MAXLINE];
    rio_t rio;
    for (i = 0; (i <= p -> maxi) && (p->nready > 0); i++) {
        connfd = p ->clientfd[i];
        rio = p->clientrio[i];
        if ((connfd > 0) && (FD_ISSET(connfd, &p->ready_set))) {
            p->nready--;
            if ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
                byte_cnt += n;
                printf("Server received %d (%d total) bytes on fd %d\n", n, byte_cnt, connfd);
                memset(response, 0, sizeof(response));

                if (!strcmp(buf, "show\n")){ //show
                    showcommand(root);
                }
                else if (!strcmp(buf, "exit\n")){ //exit
                    char errorline[10] ="exit\n";
                    strcpy(response, errorline);
                    Rio_writen(connfd, response, MAXLINE);
                    Close(connfd);
                    FD_CLR(connfd, &p->read_set);
                    p->clientfd[i] = -1;
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
            }
            else {
                Close(connfd);
                FD_CLR(connfd, &p->read_set);
                p->clientfd[i] = -1;
            }
        }
    }
}


int main(int argc, char **argv) 
{
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;  /* Enough space for any address */  //line:netp:echoserveri:sockaddrstorage
    char client_hostname[MAXLINE], client_port[MAXLINE];
    static pool pool;

    if (argc != 2) {
	    fprintf(stderr, "usage: %s <port>\n", argv[0]);
	    exit(0);
    }

    listenfd = Open_listenfd(argv[1]);
    init_pool(listenfd, &pool);
    init_tree(root);

    while (1) {
        pool.ready_set = pool.read_set;
        pool.nready = Select(pool.maxfd+1, &pool.ready_set, NULL, NULL, NULL);
        if (FD_ISSET(listenfd, &pool.ready_set)){
            clientlen = sizeof(struct sockaddr_storage);
	        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
            add_client(connfd, &pool);
            Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
            printf("Connected to (%s, %s)\n", client_hostname, client_port);
        }
        check_clients(&pool);
        FILE *fp = fopen("stock.txt", "w");
        store_txt(fp, root);
        fclose(fp);
    }
    exit(0);
}
/* $end echoserverimain */
