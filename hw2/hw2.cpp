#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sstream>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace std;

char buf[1024];
int sock,client;
int rc;
struct sockaddr_un server;

string BYE("BYE");
string command;

int main(int argc, char **argv)
{
  if(argc<2)
    return 0;

  key_t key = ftok("memory", 1);
  int shmid = shmget(key, 1024, 0644 | IPC_CREAT);
  char *data = (char*)shmat(shmid,0,0);
  data[0] = 'A';
  data[1] = 0;



  sock = socket(AF_UNIX, SOCK_STREAM, 0);
  server.sun_family = AF_UNIX;
  strcpy(server.sun_path, argv[1]);
  bind(sock, (struct sockaddr*)&server, sizeof(struct sockaddr_un));
  listen(sock, 1);


  while((client = accept(sock, 0, 0)))
  {
    if(!fork())
    {

      while((rc=read(client, buf, 1024)))
      {
        buf[rc-1] = 0;
        string sbuf(buf);
        istringstream iss(sbuf);
        iss >> command;
        data = (char*)shmat(shmid,0,0);
        if( command == "BYE" )
        {
          return 0;
        }
        else if(command == "LOCKR")
        {
          write(client,data,1);
          data[0] = command[0];
        }
      }
    }

  }

  return 0;
}
