#include <iostream>
#include <thread>
#include <vector>
#include <set>
#include "blocking_queue.h"
#include "lockfree_queue.h"
#include "serialqueue.h"

MSQueue bq;

void testpush(int t_id) {

  for (int i = 0; i < 1000; i++)
    bq.push((i + 1) + 1000 * (t_id + 1));

}

void testpop() {

  value tmp;
  for (int i = 0; i < 1000; i++)
    bq.pop(tmp);

}


void testboth(int t_id) {
  
  for (int i = 0; i < 1000; i++)
    bq.push((i + 1) + 1000 * (t_id + 1));
  
  value tmp;
  for (int i = 0; i < 1000; i++)
    bq.pop(tmp);

}


int main(int argc, char *argv[]) {

  int tmp = bq.initialize(10000);

  int n_threads = atoi(argv[1]);
  

  std::vector<std::thread> pool;

  for (int i = 0; i < n_threads; i++) {
    pool.push_back(std::thread(testpush, i));
  }

  for (auto &t: pool) {
    t.join();
  }
  
  std::cout << "Enqueue: " << bq.size() << ", expected: 10000\n";


  std::set<int> res;
  for (int i = 0; i < 10000; i++) {
    int tmp;
    bq.pop(tmp);
    res.insert(tmp);
  }

  std::cout << "After enque: " << res.size() << " unique values, expected: 10000\n";


  pool.clear();
  for (int i = 0; i < n_threads; i++) {
    pool.push_back(std::thread(testpop));
  }

  for (auto &t: pool) {
    t.join();
  }

  std::cout << "Deque: " << bq.size() << ", expected: 0\n";
  
  pool.clear();
  for (int i = 0; i < n_threads; i++) {
    pool.push_back(std::thread(testboth, i));
  }

  for (auto &t: pool) {
    t.join();
  }

  std::cout << "Both: " << bq.size() << ", expected: 0\n";

  return 0;
}


