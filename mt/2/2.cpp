#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <poll.h>
#include <string>
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

class CVset
{
public:
  pthread_mutex_t *mutex;
  int N=4;
  map<int,pthread_cond_t*> m;

  void lock()
  {
    pthread_mutex_lock(mutex);
  }
  void unlock()
  {
    pthread_mutex_unlock(mutex);
  }
  void wait(short set[])
  {
    int res;
    for(int i=0;i<N;i++)
    {
      res = res*2+set[i];
    }
    m[res] = pthread_cond_init();
    pthread_cond_wait( m[res],mutex  );
  }
  void signal(int s)
  {
    for( t in m.keys() )
    {
      int k=t;
      for(int i=s;i<N;i++)
      {
        k/=2;
      }
      if(k%2)
        pthread_cond_signal( m[k] );
    }

  }



};
