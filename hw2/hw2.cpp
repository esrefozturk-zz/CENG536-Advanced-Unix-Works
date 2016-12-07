#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sstream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstdio>
#include <sys/wait.h>
#include <cstdlib>
#include <pthread.h>


using namespace std;

string locks_shared_name("locks_shared");
string blockeds_shared_name("blockeds_shared");
string mutexes_shared_name("mutexes_shared");
string conds_shared_name("conds_shared");



key_t key_locks=1;
key_t key_blockeds=2;
key_t key_mutexes=3;
key_t key_conds=4;

int sock;
int client;
int wait_status;
char buf[1024];
int read_count;



pthread_mutex_t locks_lock;


typedef struct mystruct1
{
  int id;
  double xoff,yoff,width,height;
  char type ;
} lock;

typedef struct mystruct2
{
  int id;
  double xoff,yoff,width,height;
} blocked;

lock *locks_mem;
blocked *blockeds_mem;
pthread_mutex_t *mutexes_mem;
pthread_cond_t *conds_mem;

pthread_mutex_t *locks_mutex;
pthread_cond_t *locks_read;
pthread_cond_t *locks_write;

pthread_mutex_t *blockeds_mutex;
pthread_cond_t *blockeds_read;
pthread_cond_t *blockeds_write;


void create_shared_memorys()
{
  FILE *f;

  int locks_id;
  int blockeds_id;
  int mutexes_id;
  int conds_id;





  f = fopen(locks_shared_name.c_str(),"w");
  fclose(f);
  f = fopen(blockeds_shared_name.c_str(),"w");
  fclose(f);
  f = fopen(mutexes_shared_name.c_str(),"w");
  fclose(f);
  f = fopen(conds_shared_name.c_str(),"w");
  fclose(f);


  key_locks = ftok(locks_shared_name.c_str(), key_locks);
  key_blockeds = ftok(blockeds_shared_name.c_str(), key_blockeds);
  key_mutexes = ftok(mutexes_shared_name.c_str(), key_mutexes);
  key_conds = ftok(conds_shared_name.c_str(), key_conds);


  locks_id= shmget(key_locks, sizeof(lock)*100000, 0777 | IPC_CREAT);
  locks_mem = (lock*)shmat(locks_id,NULL,0);
  memset(locks_mem,0,sizeof(lock)*100000);

  blockeds_id= shmget(key_blockeds, sizeof(blocked)*100000, 0777 | IPC_CREAT);
  blockeds_mem = (blocked*)shmat(blockeds_id,NULL,0);
  memset(blockeds_mem,0,sizeof(blocked)*100000);

  mutexes_id= shmget(key_mutexes, sizeof(pthread_mutex_t)*100000, 0777 | IPC_CREAT);
  mutexes_mem = (pthread_mutex_t*)shmat(mutexes_id,NULL,0);
  memset(mutexes_mem,0,sizeof(pthread_mutex_t)*100000);

  conds_id= shmget(key_conds, sizeof(pthread_cond_t)*100000, 0777 | IPC_CREAT);
  conds_mem = (pthread_cond_t*)shmat(conds_id,NULL,0);
  memset(conds_mem,0,sizeof(pthread_cond_t)*100000);
}

void create_socket(char *path)
{
  struct sockaddr_un server;

  sock = socket(AF_UNIX, SOCK_STREAM, 0);
  server.sun_family = AF_UNIX;
  strcpy(server.sun_path, path);
  bind(sock, (struct sockaddr*)&server, sizeof(struct sockaddr_un));
  listen(sock, 1);
}

void create_mutexes_and_conds()
{
  pthread_mutexattr_t locks_mutex_attr;
  pthread_mutexattr_t blockeds_mutex_attr;

  pthread_condattr_t locks_read_attr;
  pthread_condattr_t locks_write_attr;
  pthread_condattr_t blockeds_read_attr;
  pthread_condattr_t blockeds_write_attr;

  locks_mutex = &(mutexes_mem[0]);
  blockeds_mutex = &(mutexes_mem[1]);
  locks_read = &(conds_mem[0]);
  locks_write= &(conds_mem[1]);
  blockeds_read = &(conds_mem[2]);
  blockeds_write = &(conds_mem[3]);



  pthread_mutexattr_init(&locks_mutex_attr);
  pthread_mutexattr_setpshared(&locks_mutex_attr,PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(locks_mutex, &locks_mutex_attr);

  pthread_mutexattr_init(&blockeds_mutex_attr);
  pthread_mutexattr_setpshared(&blockeds_mutex_attr,PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(blockeds_mutex, &blockeds_mutex_attr);

  pthread_condattr_init(&locks_read_attr);
  pthread_condattr_setpshared(&locks_read_attr, PTHREAD_PROCESS_SHARED);
  pthread_cond_init(locks_read, &locks_read_attr);

  pthread_condattr_init(&locks_write_attr);
  pthread_condattr_setpshared(&locks_write_attr, PTHREAD_PROCESS_SHARED);
  pthread_cond_init(locks_write, &locks_write_attr);

  pthread_condattr_init(&blockeds_read_attr);
  pthread_condattr_setpshared(&blockeds_read_attr, PTHREAD_PROCESS_SHARED);
  pthread_cond_init(blockeds_read, &blockeds_read_attr);

  pthread_condattr_init(&blockeds_write_attr);
  pthread_condattr_setpshared(&blockeds_write_attr, PTHREAD_PROCESS_SHARED);
  pthread_cond_init(blockeds_write, &blockeds_write_attr);

}

void agent(int client)
{
  string command;
  int xoff;
  int yoff;
  int width;
  int height;
  int lock_id=1;
  int blocked_id=1;


  while((read_count=read(client, buf, 1024)))
  {
    buf[read_count] = 0;
    string sbuf(buf);
    istringstream iss(sbuf);
    iss >> command;
    if( command == "LOCKR" )
    {
      iss >> xoff >> yoff >> width >> height;

      

    }
    else if(command == "BYE")
    {
      close(client);
      exit(0);
    }

  }
}


int main(int argc, char **argv)
{
  if(argc<2)
  {
    printf("GIVE ME SOCKET!\n");
    return 1;
  }
  create_shared_memorys();
  create_socket(argv[1]);
  create_mutexes_and_conds();

  while((client = accept(sock, 0, 0)) ) if(!fork())
  {
    agent(client);
  }

  while(wait(&wait_status)>0);
  return 0;

}
