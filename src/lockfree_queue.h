#define DEFAULT -1
#include <iostream>
#include <atomic>

typedef int value;

struct node;

struct ptr {
  node* p;
  int c;

  bool operator==(const ptr& rhs) const {
    return this->p == rhs.p && this->c == rhs.c;
  }
};

struct node {
  std::atomic<value> val;
  std::atomic<ptr> next;
};


class MSQueue {
private:
  std::atomic<ptr> head;
  std::atomic<ptr> tail;

public:
  int initialize(unsigned int maxqsize) {
    node* n = new node();
    n->val = DEFAULT;
    n->next = {nullptr, 0};
    ptr h = head.load();
    ptr t = tail.load();
    h.p = t.p = n;
    head = h;
    tail = t;
    return 1;
  }
  
  void dump() {
    ptr h = head.load();
    ptr t = tail.load();

    if (!h.p) return;
    h = h.p->next.load();

    int cnt = 0; 
    while (h.p && !(h == t)) {
      printf("Q%d: %d\n", ++cnt, h.p->val.load());
      h = h.p->next;
    }

    return;
  }
  
  int size() {
    ptr h = head.load();
    ptr t = tail.load();
    
    if (!h.p) return 0;
    h = h.p->next.load();

    int cnt = 0; 
    while (h.p && !(h == t)) {
      ++cnt;
      h = h.p->next;
    }

    return cnt;
  }

  int push(value v) {
    node* w = new node();
    w->val = v;
    w->next = {nullptr, 0};

    std::atomic_thread_fence(std::memory_order_acq_rel);
    ptr t, n;
    while (true) {
      t = tail.load();
      n = t.p->next.load();
      if (t == tail.load()) {
        if (n.p == nullptr) {
          if (t.p->next.compare_exchange_strong(n, {w, n.c + 1})) {
            break;
          }
        } else {
          tail.compare_exchange_strong(t, {n.p, t.c + 1});
        }
      }
    }

    tail.compare_exchange_strong(t, {w, t.c + 1});

    return 1;
  }
  
  
  int pop(value &item) {
    ptr h, t, n;
    
    value rtn = DEFAULT;
    while (true) {
      h = head.load();
      t = tail.load();
      n = h.p->next.load();
      if (h == head.load()) {
        if (h.p == t.p) {
          if (n.p == nullptr) {
            item = DEFAULT;
            return 0;
          }
          tail.compare_exchange_strong(t, {n.p, t.c + 1});
        } else {
          rtn = n.p->val.load();
          if (head.compare_exchange_strong(h, {n.p, h.c + 1})) {
            break;
          }
        }
      }
    }

    std::atomic_thread_fence(std::memory_order_acq_rel);
    free(h.p);
    item = rtn;

    return 1;
  }

};
