/* This is mysh.c. It will simulate a shell.
 * It has a few internal commands to pay attention two,
 * or it forks and makes a new process for external commants.
 * @author: Brian Finnerty
 */

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* This is my que that will be
 * responsible for keeping track of my
 * history
 */
struct Que{
	char* str;
	int number;
	struct Que* next;
};
typedef struct Que *Queue; 

/*This is the defenition of all
 * functions and a few static variables
 * used for convinence
 */
static Queue queue;
static int histLen;

static int ver;

Queue makeQueue(char*str,int number);
Queue destroyQueue(Queue que);
char** freeArray(int argc, char** str);
int help(int argc,char** argv);
int history(int argc,char** argv);
int quit(int argc, char** argv);
int verbose(int argc, char** argv);
char** token(char* buffer);
int checkNumber(char* str);
Queue tooBig(int histLen);
int numExists(int i);
char** getHist();
char* getHistString(int num);
int getLen();
char* getBang(char** argv);
char* trim(char* buf);
int runExec(int argc,char** argv);
int bang(int argc, char** argv);
int getArg(char** argv);
char** removeQuote(int argc, char** argv);


/* This function will be responible for creating
 * a new queue node.
 * @param char*: the string/ command it will store
 * @param int: its number in history
 * @return Queue: the new head of the queue
 */
Queue makeQueue(char* str,int number){
	
	Queue q = calloc(1, sizeof(struct Que));
	q->str = calloc(strlen(str)+1,sizeof(char));
	q->str = strncpy(q->str,str,strlen(str));
	q->number = number;
	q->next=queue;
	return q;
}


/* This function will destory the queue
 * free all memory associated with it
 * and then return a freed queue
 * @param Queue: the head of the queue
 * @return Queue: a now empty queue
 */
Queue destroyQueue(Queue que){
	if(que==NULL)
		return que;
	destroyQueue(que->next);
	free(que->str);
	que->str=NULL;
	que->next=NULL;
	free(que);
	que=NULL;
	return queue;
}


/* This function will be equiped to free up
 * and destroy arrays of strings
 * @param int: the length of the array
 * @param char**: the array to be freed
 * @return char**: the empty array
 */
char** freeArray(int argc,char** str){
	if(str==NULL)
		return NULL;
	for(int i=0;i<argc;i++){
		free(str[i]);
		str[i]=NULL;
	}
	return str;
}


/* This is an internal command that will display
 * what other commands do and their arguments/ 
 * funtionality
 * @param int: b/c prototype
 * @param char*[]: b/c prototype
 * @return: exit success since the funtion
 * ran correctly
 */
int help(int argc, char* argv[]){
	printf("\t!N-N = valid history number-bang" 
	" command calls recently used command\n");
	printf("\thelp--prints list of internal commands\n");
	printf("\thistory--prints short list of remembered entered"
		      	" commands\n");
	printf("\tquit--Cleans up memory and exits\n");
	printf("\tverbose-on|off-displays extra information\n");

	//this displays the unclear verbose instructions
	if(ver==1){
		char* b = calloc(strlen(queue->str)+1,sizeof(char));
		strcpy(b,queue->str);	
		printf("\tcommand: %s\n",b);
		free(b);
		b=NULL;
		argv = removeQuote(argc,argv);
		for(int i=0;i<argc;i++){
			printf("\t%d: %s\n",i,argv[i]);
		}
	}
	return EXIT_SUCCESS;
}


/* This function will display the history of 
 * commands that it is responible for keeping track of
 * @paran int: size of argv
 * @param char**: the token of arguments
 * @return 1: ran successfully
 */
int history(int argc, char* argv[]){

	//gets a printable list of stings
	char**myHist = getHist();
	int size = getLen(queue);
	//prints all strings
	for(int i=0;i<size;i++){
		printf("\t%s\n",myHist[i]);
	}
	//time for freeing
	if(myHist!=NULL){
		myHist = freeArray(size,myHist);
		free(myHist);
		myHist=NULL;
	}
	//prints unclear verbose
	if(ver==1){
		char* b = calloc(strlen(queue->str)+1,sizeof(char));
		strcpy(b,queue->str);
		printf("\tcommand: %s\n",b);
		free(b);
		b=NULL;
		argv = removeQuote(argc,argv);
		for(int i=0;i<argc;i++){
			printf("\t%d: %s\n",i,argv[i]);
		}
	}
	return EXIT_SUCCESS;
}


/* This function will indicate that the
 * loop must terminate
 * @param int: size of argv
 * @param char**: tokenized arguments
 * @return 1: ran successfully
 */
int quit(int argc, char** argv){
	//classic verbose
	if(ver==1){
		char* b = calloc(strlen(queue->str)+1,sizeof(char));
		strcpy(b,queue->str);
		printf("\tcommand: %s\n",b);
		free(b);
		b=NULL;
		argv = removeQuote(argc,argv);
		for(int i=0;i<argc;i++){
			printf("\t%d: %s\n",i,argv[i]);
		}
        }
	return EXIT_SUCCESS;
}


/* This function will turn on and turn off the
 * verbose funtionality as long as the user entered the 
 * internal command correctly.
 * Verbose displays extra info about each command
 * @param int: size of argv
 * @param char*[]: tokenized arguments
 * @return 1: ran successfully
 */
int verbose(int argc,char* argv[]){
	char* in = calloc(strlen(queue->str)+1,sizeof(char));
	strcpy(in,queue->str);
	//checks for second argument
	if(argv[1]!=NULL){
		//dea;s with turning it on/off
		if(strcmp(argv[1],"on")==0){
			free(in);
			in=calloc(100,sizeof(char));
			strncpy(in,"verbose on",strlen("verbose on"));
			ver =1;
		}
		else if(strcmp(argv[1],"off")==0){
			free(in);
			in=calloc(100,sizeof(char));
			strncpy(in,"verbose off",strlen("verbose off"));
			ver =0;
		}
		else{
			fprintf(stderr,"usage: verbose on|off\n");
			return 0;
		}
	}
	else{
		fprintf(stderr,"usage: verbose on|off\n");
		return 0;
	}
	
	if(in!=NULL){
		free(in);
		in=NULL;
	}
	//verbose bullshit
	if(ver==1){
		char* b = calloc(strlen(queue->str)+1,sizeof(char));
		strcpy(b,queue->str);
		printf("\tcommand: %s\n",b);
		free(b);
		b=NULL;
		argv = removeQuote(argc,argv);
		for(int i=0;i<argc;i++){
			printf("\t%d: %s\n",i,argv[i]);
		}
	}

	return EXIT_SUCCESS;
}


/* This function will remove quotation
 * marks from the arguments
 * @param int: size of argv
 * @param char**: token arguments
 * @return char**: no arguments
 */
char** removeQuote(int argc,char**argv){
	//this loop throguh every argument
	for(int i=0;i<argc;i++){
		int head=0;
		//loops throguh every string
		for(int j=0;j<=(int)strlen(argv[i]);j++){
			if(argv[i][j]!='\''&&argv[i][j]!='"'){
				argv[i][head]=argv[i][j];
				head++;
			}
		}
	}
	//now no quotes
	return argv;
}


/* This function will take a string and tokenize it
 * @param char*: a string to be tokenized
 * @return char**: this is the tokenized arguments
 */
char** token(char* buf){
	// sets up array
	char** arg = calloc(10,sizeof(char*));
	int counter = 0;
	int argSize =10;
	int i=0;

	//loops through entire string
	while(i<(int)strlen(buf)){
		//creates new string
		char* tok = calloc(20,sizeof(char));
		int tokSize = 20;
		int head = 0;
		int sinQuote = 0;
		int dubQuote = 0;
		int run =1;
		//loops until it finds a reason to make a
		//new argument
		while(run ==1){
			if(i>=(int)strlen(buf)){
				run=0;
				i++;
				continue;
			}
			//see regular characters or a quoted space
			if((buf[i]!=' '&&buf[i]!='\n')||(buf[i]==' ' 
				&& (sinQuote==1|| dubQuote==1))){
				if(buf[i]=='"')
					dubQuote = (dubQuote==1)?0:1;
				else if(buf[i]=='\'')
					sinQuote = (sinQuote==1)?0:1;
				//bang is fancy
				if(buf[i]=='!' && i==0){
					tok[0]='!';
					tok[1]='\0';
					head=2;
					run=0;
				}
				else{
					//provides new size to string
					if(head==tokSize-1){
						tokSize = tokSize*2;
						char* reTok = 
						realloc(tok,
						tokSize*sizeof(char));
						if(reTok!=NULL){
							tok = reTok;
						}
					}
					tok[head]=buf[i];
					head++;
					
				}
				i++;
			}
			else{
				tok[head]='\0';
				run = 0;
				i++;
			}
		}

		//out of making a token, time to add to array
		if(head>0){
			tok[head]='\0';
			arg[counter]=tok;
			counter++;
			//makes array bigger
			if(counter==argSize-1){
				argSize=argSize*2;
				char** tmp = realloc(arg,
				argSize*sizeof(char*));
				if(tmp!=NULL){
					arg=tmp;
					arg[counter]=NULL;
				}
			}
		}
		//frees strings that have nothing inside them
		else{
			free(tok);
			tok=NULL;
		}
	}
	if(counter<argSize)
		arg[counter]=NULL;
	return arg;
}


/* This function will check to see if this string is
 * actually a number, if so then it returns 1,other wise 0
 * @param char*: a string
 * @return int: yes all numbers or no
 */
int checkNumber(char* str){
	for(int i=0;i<(int)strlen(str);i++){
		if(str[i]=='0'){}
		else if(str[i]=='1'){}
		else if(str[i]=='2'){}
		else if(str[i]=='3'){}
		else if(str[i]=='4'){}
		else if(str[i]=='5'){}
		else if(str[i]=='6'){}
		else if(str[i]=='7'){}
		else if(str[i]=='8'){}
		else if(str[i]=='9'){}
		else
			return 0;
	}
	if((int)strlen(str)==0)
		return 0;
	else
		return 1;
}


/* This function will check to see if my
 * queue is keeping track of too much history
 * If it is then it will destroy the last node on
 * the list
 * @param int: max length of the queue
 * @return Queue: the Queue head
 */
Queue tooBig(int histLen){
	Queue head = queue;
	int counter =0;
	if(head==NULL)
		return NULL;
	counter++;
	//runs to the end
	while(head->next!=NULL){
		head = head->next;
		counter++;
	}
	//checks to see if the size if too big
	if(counter<=histLen){
		return queue;
	}
	int last = (int)head->number;
	last++;
	head=queue;
	//finds the node to get rid off
	while(head->next!=NULL){
		if(last==(int)(head->number))
			break;
		else if(last==head->number)
			return queue;
		else
			head=head->next;
	}
	//free'sthat node
	free(head->next->str);
	head->next->next=NULL;
	free(head->next);
	head->next=NULL;
	
	return queue;
}


/* This function will check to see if the given
 * number exits inside of the queue
 * returns 1 if it does, 0 if it does not
 * @param int: history number to look for
 * @return: 1 or 0
 */
int numExists(int i){
	int tru = 0;
	Queue head = queue;
	//attempts to find the number
	while(head!=NULL){
		if((int)head->number==i){
			tru=1;
			break;
		}
		head=head->next;
	}
	return tru;
}


/* This funtion will gather the entire history
 * of the queue and create a list of all strings
 * @return char**: the list of strings that are history
 */
char** getHist(){
	int counter = 0;
	Queue head = queue;
	//counts all nodes
	while(head!=NULL){
		counter++;
		head=head->next;
	}
	if(counter==0)
		return NULL;
	head=queue;
	//sets up string arrray and then puts all strings inside of it
	char** hist = calloc(counter, sizeof(char*));
	for(int i=counter-1;i>=0;i--){
		hist[i] = calloc(strlen(head->str)+8,sizeof(char));
		sprintf(hist[i],"%d ",(int)head->number);
		strncat(hist[i],head->str,strlen(head->str));
		head=head->next;
	}
	head=NULL;
	return hist;
}


/* This function will get a specific string from
 * one queue node. This is useful for the bang command
 * @param int: the node to look for
 * @return char*: the string from that node
 */
char* getHistString(int num){
	Queue head = queue;
	//loops throguh queue
	while(head!=NULL){
		//if the number is found
		if(num==(int)head->number){
			char* s = calloc(strlen(head->str)+1,sizeof(char));
			strncpy(s,head->str,strlen(head->str));
			return s;
		}
		else{
			head=head->next;
		}
	}
	//shouldn't reach here since I already checked to see if number
	//was in the queue
	return NULL;
}


/* This function will get the 
 * entire length of the queue
 * @return int: size of the queue
 */
int getLen(){
	int size =0;
	Queue head = queue;
	while(head!=NULL){
		size++;
		head=head->next;
	}
	return size;
}


/* This function will recieve arguments and then
 * return the string that is associated with the queue
 * number it is looking for
 * @param char**: the tokenized arguments
 * @return char*: either the string from history or the bang
 * command
 */
char* getBang(char** argv){
	char* buffer = calloc(strlen(queue->str)+1,sizeof(char));
	strcpy(buffer,queue->str);
	//checks to see if there is anything after
	if(argv[1]!=NULL){
		int isNum = checkNumber(argv[1]);
		//checks to see if the string is actually a number
		if(isNum == 1){
			char* enpnt;
			int space = (int)strtol(argv[1],&enpnt,10);
			// checks to see if the number is inside the queue
			if(numExists(space)==1){
				char* str = getHistString(space);
				free(buffer);
				buffer = calloc(strlen(str)+1,sizeof(char));
				buffer = strncpy(buffer,str,strlen(str));
				free(str);
				str=NULL;
			}
			else{
				return buffer;
			}
		}
		else{
			return buffer;
		}
	}
	else{
		return buffer;
	}
	return buffer;
}


/* This function will take all newline characters
 * and leading spaces off of the string
 * @param char*: the string that needs trimming
 * @return char*: the trimmed string
 */
char* trim(char* buf){
	char* send = calloc(strlen(buf)+1,sizeof(char));
	int head =0;
	int leadingSpace = 1;
	if(buf==NULL)
		return buf;
	// this walks through and trims the string
	// ignoring spaces and newline characters
	for(int i=0;i<(int)strlen(buf);i++){
		if(leadingSpace ==1 && buf[i]==' ')
		{}
		else{
			if(buf[i]!=' ')
				leadingSpace=0;
			if(buf[i]=='\n'){
				send[head]='\0';
				head++;
			}
			else{
				send[head]=buf[i];
				head++;
			}
		}
	}
	send[head]='\0';
	//this is needed because reasons
	if(buf!=NULL){
		free(buf);
		buf=NULL;
	}
	return send;
}


/* This function will run the exec command
 * @param int: size of argv
 * @param char**: tokens of arguments
 * @return 1; ran successfully
 */
int runExec(int argc,char**argv){
	//makes and breaks string to be formed into arguments
	char* buffer = calloc(strlen(queue->str)+1,sizeof(char));
	strncpy(buffer,queue->str,strlen(queue->str));
	buffer = trim(buffer);
	char** a = token(buffer);
	a = removeQuote(getArg(a),a);
	argv = removeQuote(argc,argv);
	//set up for new process
	pid_t pid;
	int status;
	pid=fork();

	if(pid==-1){
		perror("fork failure:");
	}
	else if(pid>0){//parent
		pid = waitpid(pid,&status,0);
		if(pid<0)
			perror("wait failure:");
		else{
			//guess all things go into queue even bad commands
			char* buf = calloc(strlen(queue->str)+1,sizeof(char));
			strcpy(buf,queue->str);
			buf= trim(buf);
			//verbose
			if(ver==1){
				printf("\tcommand: %s\n",buf);
				printf("\tinput command tokens:\n");
				for(int i=0;i<argc;i++){
					printf("\t%d: %s\n",i,argv[i]);
				}
				printf("\twait for pid %d: %s\n",(int)pid,argv[0]);
				printf("\texecvp: %s\n",argv[0]);
			}
			free(buf);
			buf=NULL;
		}
		if(WIFEXITED(status)){
			int exit_status= WEXITSTATUS(status);
			if(ver==1)
				printf("\tcommand status: %d\n",exit_status);
		}
	}
	else{//child
		execvp(a[0],a);
		//if exec fails
		fprintf(stderr,"%s:No such file or directory\n",a[0]);
		if(buffer!=NULL){
			free(buffer);
			buffer=NULL;
		}
		if(a!=NULL){
			int size = getArg(a);
			a=freeArray(size,a);
			free(a);
			a=NULL;
		}
		if(argv!=NULL){
			int size = getArg(argv);
			argv=freeArray(size,argv);
			free(argv);
			argv=NULL;
		}
		if(queue!=NULL){
			queue=destroyQueue(queue);
			queue=NULL;
		}
		_exit(1);
	}
	//cleanup
	if(buffer!=NULL){
		free(buffer);
		buffer=NULL;
	}
	
	if(a!=NULL){
		int size = getArg(a);
		a=freeArray(size,a);
		free(a);
		a=NULL;
	}

	return EXIT_SUCCESS;
}


/* This funtion will run the bang command
 * goes into history to find the stuff it needs
 * and then running as normal
 * @param int: size of argv
 * @param char**: token arguments
 * @return 1: success
 */
int bang(int argc, char** argv){
	(void)argc;
	char* buf = getBang(argv);
	// ignore the bang command
	if(buf[0]=='!'){
		fprintf(stderr,"This bang command is incorrect"
		" and doesn't exit in current history\n");
		free(buf);
		buf=NULL;
		Queue head = queue;
		queue = head->next;
		free(head->str);
		head->str=NULL;
		head->next=NULL;
		free(head);
		head=NULL;
		return 1;
	}
	free(queue->str);
	queue->str=calloc(strlen(buf)+1,sizeof(char));
	strcpy(queue->str,buf);
	argv=token(buf);
	int argSize=0;
	if(argv!=NULL)
		argSize = getArg(argv);
	printf("argSize: %d\n",argSize);

	
	if(buf!=NULL){
		free(buf);
		buf=NULL;
	}
	// runs commands as normal
	if(strcmp(argv[0],"help")==0){
		int i =help(argSize,argv);
		if(argv!=NULL){
			argv=freeArray(argSize,argv);
			free(argv);
			argv=NULL;
		}
		return i;
	}
	else if(strcmp(argv[0],"history")==0){
		int re = history(argSize,argv);
		if(argv!=NULL){
			argv=freeArray(argSize,argv);
			free(argv);
			argv=NULL;
		}
		return re;
	}
	else if(strcmp(argv[0],"verbose")==0){
		int re =verbose(argSize,argv);
		if(argv!=NULL){
			argv=freeArray(argSize,argv);
			free(argv);
			argv=NULL;
		}
		return re;
	}
	else if(strcmp(argv[0],"quit")==0){
		int re=quit(argSize,argv);
		if(argv!=NULL){
			argv=freeArray(argSize,argv);
			free(argv);
			argv=NULL;
		}
		return re;
	}
	else{
		int re = runExec(argSize,argv);
		if(argv!=NULL){
			argv=freeArray(argSize,argv);
			free(argv);
			argv=NULL;
		}
		return re;
	}
}


/* This will get the size of an argument array
 * @param char**: tokenized arguments
 * @return int: the size of argv
 */
int getArg(char** argv){
	int counter = 0;
	while(argv[counter]!=NULL){
		counter++;
	}
	return counter;
}


/* This will check to see if command arguments
 * actually match and stuff
 * @param int: size of argv
 * @param char**: command arguments to set up
 * @return 1 or 0; tells program whether it should run
 */
int checkArgs(int argc, char** argv){
	if(argc==1)
		return 1;
	else{
		if(argc==2 && strcmp(argv[1],"-v")==0){
			ver=1;
			return 1;
		}
		else if(argc==3){
			if(strcmp(argv[1],"-v")==0){
				if(checkNumber(argv[2])==1){
					char* e;
					histLen = (int)strtol(argv[2],&e,10);
					return 1;
				}
			}
		}
		else if(argc==4){
			if(strcmp(argv[1],"-v")==0||strcmp(argv[3],"-v")==0){
				ver=1;
			}
			if(strcmp(argv[1],"-h")==0){
				if(checkNumber(argv[2])==1){
					char* e;
					histLen = (int)strtol(argv[2],&e,10);
					return 1;
				}
			}
			if(strcmp(argv[2],"-h")==0){
				if(checkNumber(argv[3])==1){
						char*e;
						histLen=(int)strtol(argv[3],&e,10);
						return 1;
				}
			}
		}
		return 0;
	}
}



int main(int argc, char* argv[]){
	
	//initial set up
	char* buffer = NULL;
	size_t getL = 0;
	histLen = 10;
	int counter = 1;
	int goOn = checkArgs(argc,argv);
	
	//are command lines correct
	if(goOn ==0){
		fprintf(stderr,"usage: mysh [-v] [-h pos_num]\n");
		return EXIT_FAILURE;
	}
	
	//more setup
	char** args = NULL;
	queue = NULL;
	
	//here is the start
	printf("mysh[%d]> ",counter);
	getline(&buffer,&getL,stdin);
	while(!feof(stdin)){
		//fatal error stuff
		if(ferror(stdin)){
			perror("");
			break;
		}
		// if somehow things screwed up
		if(strlen(buffer)==0){
			//make print statement for null string
			printf("mysh[%d]> ",counter);
			getline(&buffer,&getL,stdin);
			continue;
		}
		//ignore empty stuff
		if(buffer[0]=='\n'){
			printf("mysh[%d]> ",counter);
			getline(&buffer,&getL,stdin);
			continue;
		}
		//breaks up input string
		buffer = trim(buffer);
		
		queue = makeQueue(buffer,counter);
		queue = tooBig(histLen);
		printf("setting token\n");
		args = token(buffer);
		int argSize = getArg(args);
		
		//runs commands
		if(buffer[0]=='!'){
			bang(argSize,args);
			free(buffer);
			buffer=NULL;
			args=freeArray(argSize,args);
			free(args);
			args=NULL;
		}
		else if(strcmp(args[0],"help")==0){
			help(argSize,args);
		}
		else if(strcmp(args[0],"history")==0){
			history(argSize,args);
		}
		else if(strcmp(args[0],"quit")==0){
			quit(argSize,args);
			break;
		}
		else if(strcmp(args[0],"verbose")==0){
			if(args[1]!=NULL){
				if(strcmp(args[1],"on")==0)
					verbose(argSize,args);
				else if(strcmp(args[1],"off")==0)
					verbose(argSize,args);
				else
					fprintf(stderr,"usage: verbose on|off\n");
			}
			else
				fprintf(stderr,"usage: verbose on|off\n");
		}
		else{
			if(buffer!=NULL){
				free(buffer);
				buffer=NULL;
			}
			runExec(argSize,args);
		}
		//frees that happen for safety
		//and for my sanity
		if(args!=NULL){
			int size = getArg(args);
			args=freeArray(size,args);
			free(args);
			args=NULL;
		}
		if(buffer!=NULL){
			free(buffer);
			buffer=NULL;
		}
		if(queue!=NULL)
			counter = (int)queue->number+1;
		//gets next command
		printf("mysh[%d]> ",counter);
		getline(&buffer,&getL,stdin);
	}
	//more safety/sanity frees
	if(args!=NULL){
		int size = getArg(args);
		args=freeArray(size,args);
		free(args);
		args=NULL;
	}

	if(queue!=NULL){
		queue = destroyQueue(queue);
		queue=NULL;
	}
	if(buffer!=NULL){
		free(buffer);
		buffer=NULL;
	}

	return EXIT_SUCCESS;

}
