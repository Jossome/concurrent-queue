#include <stdlib.h>
#include <mutex>

class BlockingQueue {
private:
  std::mutex qlock;
  unsigned int qsize;
  unsigned int head;
  unsigned int tail;
  int *items;

public:

  int initialize(unsigned int maxqsize) {
    qlock.lock();
    qsize = maxqsize+1;
    items = (int *) calloc(qsize, sizeof(int));
    head = 0;
    tail = 0;
    qlock.unlock();
  }

  void dump() {
    qlock.lock();
    int i = (head + 1) % qsize;
    int e = (tail + 1) % qsize;

    while(i != e) {
      printf("Q%d: %d\n", i, items[i]);
      i = (i + 1) % qsize;
    }
    qlock.unlock();
  }
  
  int size() {
    qlock.lock();
    int i = (head + 1) % qsize;
    int e = (tail + 1) % qsize;

    int cnt = 0;
    while(i != e) {
      cnt++;
      i = (i + 1) % qsize;
    }
    qlock.unlock();
    return cnt;
  }

  int push(int item) {
    qlock.lock();
    if((tail + 1) % qsize == head) {
      // queue is full
      qlock.unlock();
      return 0;
    }

    tail = (tail + 1) % qsize;
    items[tail] = item;
    qlock.unlock();

    return 1;
  }

  int pop(int &item) {
    qlock.lock();
    if(head == tail) {      
      // queue is empty
      qlock.unlock();
      return 0;
    }

    head = (head + 1) % qsize;
    item = items[head];
    qlock.unlock();

    return 1;
  }  
};
