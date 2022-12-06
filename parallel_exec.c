#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <unistd.h>
#include <time.h>

#define TAG_ASK_FOR_JOB 1
#define TAG_JOB_DATA 2
#define TAG_STOP 3
#define TAG_JOB_DONE 4
#define TAG_ACK 5

// max cmd size
#define MAX_MSG_SIZE 1000
#define MSG_SIZE 1

/*** the structure representing a target
*  name : name of the target
*  level : level in dependency tree
*  deps : prerequisites of the target
*  num_deps : #prerequisites
*  is_made : =1 if the target is made ie its command is executed
*  cmd : the command of the target
*  desc : buffer to store deps before traitement
***/
typedef struct target{
  char *name;
  int level;
  struct target **deps;
  size_t num_deps;
  int is_made ;
  char *cmd ;
  char *desc ;
}Target;

/*** the structure representing a tree
*  root : root target
*  num_targets : #targets in tree
*  num_levels : #levels in tree ie max level
*  targets : all targets of tree
***/
typedef struct tree{
  Target *root;
  size_t num_targets;
  size_t num_levels ;
  Target **targets;
}Tree;

/**
 * @brief Create a target object
 * @return Target*
 */
Target* create_target(char *name,char *cmd,char *desc){
  Target *target = (Target*)malloc(sizeof(Target));
  target->name=name;
  if((cmd!=NULL) & (cmd!=" ")) target->cmd=cmd;
  else target->cmd=NULL;
  if((desc!=NULL) & (desc!=" ")) target->desc=desc;
  else target->desc=NULL;
  target->deps=NULL;
  target->num_deps=0;
  target->is_made=0;
  return target;
}
/**
 * @brief compare two targets based on their levels
 * @return 1 if a's level is strictly lesser than b's level
 */
int compare_targets(const void *a, const void *b){
  return (*(Target**)a)->level<(*(Target**)b)->level;
}
/**
 * @brief add a target to a tree
 */
void add_target(Tree* tree,Target* target){
  Target** t = malloc(sizeof(Target*)*(tree->num_targets+1));
  memcpy((void*)t,(void*)tree->targets,sizeof(Target*)*tree->num_targets);
  tree->targets = t;
  tree->targets[ tree->num_targets ] = target;
  tree->num_targets++;
}
/**
 * @brief add a child to parent's prerequisites
 */
void add_child(Target* parent,Target* child){
  parent->deps = (Target**)realloc( (void*)parent->deps, sizeof(Target*)*(1+parent->num_deps) );
  parent->deps[ parent->num_deps ] = child;
  parent->num_deps++;
}

/**
 * @brief search a target by name in a tree
 * @return Target* if found or null otherwise
 */
Target* find_target(Tree *tree,char *name){
  int i = 0;
  Target **t = tree->targets;
  while (i < tree->num_targets) {
	int r = strcmp((*(t+i))->name,name);
	if(r==0){
	  return *(t+i);
	}
	i++;
  }
  return NULL;
}
/**
 * @brief routines to change shell print color
 */
void red () {
  printf("\033[1;31m");
}

void green() {
  printf("\033[1;32m");
}

void reset () {
  printf("\033[0m");
}

/**
 * @brief print a linear representation of a tree
 */
void show_linear(Tree* tree){
  reset();
  printf("Linear representation :\n");
  int i = 0;
  Target **t = tree->targets;
  while (i < tree->num_targets) {
	printf("target name --> %s\n",(*(t+i))->name);
	printf("target cmd ---> %s\n",(*(t+i))->cmd);
	printf("target deps---> %s\n",(*(t+i))->desc);
	i++;
  }
}

/**
 * @brief print a tree in a a tree format
 */
void show_rec(Target* target,int depth,int last){
  green();
  printf("[%d]",target->level);
  reset();
  for (int j = 0; j < depth; ++j) printf ("│  ");
  if(last) {
	printf("└── ");
	green();
	printf("%s \n",target->name);
  }
  else {
	printf("├── ");
	green();
	printf("%s \n",target->name);
  }
  int i = 0;
  Target **t = target->deps;
  while (i < target->num_deps) {
	if((i+1)==target->num_deps) {
	  show_rec(*(t+i),depth+1,1);
	}
	else show_rec(*(t+i),depth+1,0);
	i++;
  }
}

/**
 * @brief this routine link targets with each other following
 *       the parent/child concept
 */
void handle_dependencies(Tree* tree){
  char *buffer = NULL;
  char *name =NULL;
  int i = 0;
  Target **t = (Target**)malloc(tree->num_targets*sizeof(Target*));
  t = (Target**)memcpy(t,tree->targets,tree->num_targets*sizeof(Target*));
  Target *_t = NULL;
  int max = tree->num_targets;
  while (i < max) {
	if((*(t+i))->desc){
	  buffer = strdup((*(t+i))->desc);
	  name = strtok(buffer, " ");
	  while(name!=NULL){
		_t = find_target(tree,name);
		if(!_t){
		  Target *__t = create_target(name,NULL,NULL);
		  add_target(tree,__t);
		  _t=__t;
		}
		_t->level=(*(t+i))->level+1;
		if(tree->num_levels < _t->level) tree->num_levels= _t->level;
		add_child(*(t+i),_t);
		name = strtok(NULL, " ");
	  }
	}
	i++;
  }

}
/**
 * @brief this routine executes sequentially (at the same node) the commands
 *   of all targets present in tree in descending order with respect to their level
 *  This oredering guarentees the priority of prerequisites
 */
int execute(Tree* tree){
  red();
  printf("[EXECUTION]");
  reset();
  printf(" \n");
  qsort(tree->targets, tree->num_targets ,sizeof(Target*) , compare_targets);
  int i =0 ;
  Target **t = tree->targets;
  while (i < tree->num_targets) {
	if((*(t+i))->cmd) system((*(t+i))->cmd);
	i++;
  }
}

/**
 * @brief this function is executed by the master process only
 *
 */
void master (char** argv) {
	// parsing of makefile
	FILE *fp = fopen(argv[1] , "r");
	char *line = NULL;
	size_t len = 0;
	ssize_t read =0;
	int init = 1 ;

	if (fp == NULL)
		exit(EXIT_FAILURE);

	char *buffer = NULL;
	char *name = NULL;
	char *deps = NULL;
	char *cmd = NULL;
	Target* target =NULL;
	Tree *dep_Tree = (Tree*)malloc(sizeof(Tree));
	dep_Tree->num_targets=0;

	while ((read = getline(&line, &len, fp)) != -1) {
		buffer = strdup(line);
		name = strtok(buffer," :");
		if (name != NULL && name[0] != 10 && name[0] != 13) {
			deps = strtok(NULL, ":\n");
			read = getline(&line, &len, fp);
			if (read == -1){
				break;
			}
			buffer = strdup(&line[1]);
			cmd = strtok(buffer,"\n");

			target = create_target(name,cmd,deps);
			if(init){
				target->level=1;
				dep_Tree->root = target;
				dep_Tree->num_levels=1;
				init = 0 ;
			}

		add_target(dep_Tree,target);
		}
	}
	handle_dependencies(dep_Tree);
	show_rec(dep_Tree->root,0,1);
	reset();
	printf("\n");
	qsort(dep_Tree->targets, dep_Tree->num_targets ,sizeof(Target*) , compare_targets);

	Target** t= dep_Tree->targets;

	// the mpi routine starts here

	MPI_Status stat, stat2;
	// count how many processes( at the same level)  are not yet finished
	int unfshd_procs = 0 ;

	size_t current_level = dep_Tree->num_levels ;
	size_t max_targets = dep_Tree->num_targets;
	int i = 0;

	// Buffers to send/recv messages
	char msg_buffer ='x';
	char buff ;
	char* _buffer;  // for long messages

	int size;
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	// 1st condition ---> there are unprocessed jobs
	// 2nd condition ---> there are slaves still working on jobs

	while ((i < max_targets) || (size >1)) {

		// if cmd is null we jump directlyto next cmd
		if((i<max_targets)&&(!((*(t+i))->cmd))){
			i++;
			continue;
		}
		// before processing jobs at curent_level-1 all jobs at curent_level must be finished

		if((i<max_targets)&&(*(t+i))->level != current_level) {
			if( unfshd_procs > 0 ){
				MPI_Probe(MPI_ANY_SOURCE, TAG_JOB_DONE, MPI_COMM_WORLD, &stat);
				int slave_rank = stat.MPI_SOURCE;
				MPI_Recv(&buff,MSG_SIZE, MPI_CHAR, slave_rank, TAG_JOB_DONE, MPI_COMM_WORLD, &stat2);
				MPI_Send(&msg_buffer,MSG_SIZE,MPI_CHAR, slave_rank, TAG_ACK, MPI_COMM_WORLD);
				unfshd_procs--;
			}else
			{
				current_level--;
			}
		}
		else {
			// send/recv event listener
			MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
			int slave_rank = stat.MPI_SOURCE;
			if(stat.MPI_TAG==TAG_JOB_DONE){
				MPI_Recv(&buff,MSG_SIZE, MPI_CHAR, slave_rank, TAG_JOB_DONE, MPI_COMM_WORLD, &stat2);
				MPI_Send(&msg_buffer,MSG_SIZE,MPI_CHAR, slave_rank, TAG_ACK, MPI_COMM_WORLD);
				unfshd_procs--;
			}
			else{
				MPI_Recv(&buff,MSG_SIZE, MPI_CHAR, slave_rank, TAG_ASK_FOR_JOB, MPI_COMM_WORLD, &stat2);
				// there still jobs unprocessed
				if (i<max_targets) {
					_buffer = strdup((*(t+i))->cmd);
					MPI_Send(_buffer, MAX_MSG_SIZE, MPI_CHAR ,slave_rank, TAG_JOB_DATA, MPI_COMM_WORLD);
					i++;
					unfshd_procs++;
				// no more jobs to process
				} else {
					MPI_Send (&msg_buffer,MSG_SIZE, MPI_CHAR, slave_rank , TAG_STOP , MPI_COMM_WORLD);
					size--;
				}
			}
		}
	}
}

/**
 * @brief the routine executed by slave processes
 *
 */
void slave () {

	int stopped = 0;
	MPI_Status stat , stat2 ;

	// buffers to send /recv messages
	char msg_buffer ='x';
	char buff ;
	char* _buffer = malloc(MAX_MSG_SIZE*sizeof(char));
	int my_rank ;

	// To mesure the execution time
	float temps;

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	while(stopped==0) {
		// Here we send a message to the master asking for a job
		MPI_Send (&msg_buffer,MSG_SIZE, MPI_CHAR, 0 , TAG_ASK_FOR_JOB , MPI_COMM_WORLD ) ;
		MPI_Probe (0 , MPI_ANY_TAG, MPI_COMM_WORLD , &stat) ;
		if ( stat.MPI_TAG == TAG_JOB_DATA ) {
			// Retrieve job data from master into _buffer
			MPI_Recv (_buffer,MAX_MSG_SIZE, MPI_CHAR , 0, TAG_JOB_DATA , MPI_COMM_WORLD , &stat2 ) ;
			// printf("[%d] received %s\n",my_rank,_buffer);

			// We mesure the execution time of the system() command
			system(_buffer);
			
			// send rack to master
			MPI_Send (&msg_buffer,MSG_SIZE, MPI_CHAR , 0 , TAG_JOB_DONE, MPI_COMM_WORLD);
			MPI_Recv (&buff,MSG_SIZE, MPI_CHAR , 0, TAG_ACK , MPI_COMM_WORLD , &stat2 ) ;
		}else{
			// We got a stop message we have to retrieve it by using MPI_Recv
			// But we can ignore the data from the MPI_Recv call
			MPI_Recv (&buff,MSG_SIZE, MPI_CHAR, 0, TAG_STOP , MPI_COMM_WORLD , &stat2);
			stopped = 1;
		}
	}
}

int main(int argc, char **argv) {

	int rank;
	MPI_Init(&argc , & argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	double t1, t2; 
	t1 = MPI_Wtime(); 
	if (rank == 0) {
		master (argv) ;
	} else {
		slave () ;
	}
	MPI_Barrier(MPI_COMM_WORLD);
	t2 = MPI_Wtime(); 
	if(rank==0) printf( "Elapsed time is %f\n", t2 - t1 ); 
	MPI_Finalize () ;
}
