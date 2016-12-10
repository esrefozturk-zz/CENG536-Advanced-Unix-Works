#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <poll.h>
#include <string>

using namespace std;

const char *comm[] = {"./1.py", "./2.py", "./3.py", NULL};
int allbroadcast(const char *commands[])
{
  int i=0;
  vector<int> reads;
  vector<int> writes;
  while(1)
  {

    if(commands[i] == NULL)
      break;
    int asd[2];
    int qwe[2];

    pipe(asd);
    pipe(qwe);

    if(!fork())
    {
      dup2(asd[0],0);close(asd[0]);
      dup2(qwe[1],1);close(qwe[1]);
      execv(commands[i],NULL);

    }
    else
    {
      close(asd[1]);
      close(qwe[0]);
      writes.push_back(asd[1]);
      reads.push_back(qwe[0]);
    }

    i++;
  }
  //cout << "slm" << endl;
  struct pollfd fds[reads.size()];

  for(int i=0;i<reads.size();i++)
  {
    fds[i].fd = reads[i];
  }
  char buf[102];
  while(1)
  {
    //cout << "heyt" << endl;
    int fd = poll(fds,0,0);
    //cout << fd<< endl;
    if(fd != -1)
    {

      int rc=read(fd,buf,100);
      cout << "--->" << fd << " " << buf <<  endl;
      for(int i=0;i<writes.size();i++)
      {
        if( reads[i] != fd )
        {
          write(writes[i],(string(buf)+'\n').c_str(),(string(buf)+'\n').size());
        }
      }
    }

  }

  return 0;
}

int main()
{
  allbroadcast(comm);
  return 0;
}
