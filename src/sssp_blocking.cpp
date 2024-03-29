/* -*- mode: c++ -*- */
#define BLACK 27
#define WHITE 12

#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <thread>
#include <pthread.h>
#include <atomic>
#include <vector>
#include "simplegraph.h"
#include "blocking_queue.h"
#include "Timer.h"

const int INF = INT_MAX;
int n_threads = 0;
// std::atomic_int counter{0};

void sssp_init(SimpleCSRGraphUII g, unsigned int src) {
  for(int i = 0; i < g.num_nodes; i++) {
    g.node_wt[i] = (i == src) ? 0 : INF;
  }
}

bool sssp(SimpleCSRGraphUII g, BlockingQueue *q, int *marker, int t_id) {
  bool changed = false;
  int node;

  while (true) {
    
    while (q->pop(node)) {
      marker[t_id] = BLACK;
      //counter.exchange(0);
      for(unsigned int e = g.row_start[node]; e < g.row_start[node + 1]; e++) {
        
        while (true) {
          unsigned int dest = g.edge_dst[e];
          int distance = __atomic_load_n(&(g.node_wt[node]), __ATOMIC_SEQ_CST) + g.edge_wt[e];
          int prev_distance = __atomic_load_n(&(g.node_wt[dest]), __ATOMIC_SEQ_CST);

          if (distance == __atomic_load_n(&(g.node_wt[node]), __ATOMIC_SEQ_CST) + g.edge_wt[e]) {
            if (prev_distance == __atomic_load_n(&(g.node_wt[dest]), __ATOMIC_SEQ_CST)) {
              if(prev_distance > distance) {
                if (__atomic_compare_exchange_n(&(g.node_wt[dest]), &prev_distance, distance, true, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
        	        if (!q->push(dest)) {
        	          fprintf(stderr, "ERROR: Out of queue space.\n");
        	          exit(1);
                  }
                  changed = true;
                  break;
                }
              } else {
                break;
              }
            }
          }
        } // while
      }
    }

    int i = 0;
    for (i = 0; i < n_threads; i++) {
      if (marker[t_id] == BLACK) break;
    }

    if (i == n_threads) break;
   
    marker[t_id] = WHITE;

//     while (true) {
//       int prev = counter.load();
//       if (counter.compare_exchange_weak(prev, prev + 1)) break;
//     }
//     if (counter.load() == n_threads) break;
    
  }

}

void write_output(SimpleCSRGraphUII &g, const char *out) {
  FILE *fp; 
  
  fp = fopen(out, "w");
  if(!fp) {
    fprintf(stderr, "ERROR: Unable to open output file '%s'\n", out);
    exit(1);
  }

  for(int i = 0; i < g.num_nodes; i++) {
    int r;
    if(g.node_wt[i] == INF) {
      r = fprintf(fp, "%d INF\n", i);
    } else {
      r = fprintf(fp, "%d %d\n", i, g.node_wt[i]);
    }

    if(r < 0) {
      fprintf(stderr, "ERROR: Unable to write output\n");
      exit(1);
    }
  }
}

int main(int argc, char *argv[]) 
{
  if(argc != 4) {
    fprintf(stderr, "Usage: %s inputgraph outputfile n_threads\n", argv[0]);
    exit(1);
  }

  SimpleCSRGraphUII input;
  BlockingQueue sq;

  if(!input.load_file(argv[1])) {
    fprintf(stderr, "ERROR: failed to load graph\n");
    exit(1);
  } 

  printf("Loaded '%s', %u nodes, %u edges\n", argv[1], input.num_nodes, input.num_edges);

  /* if you want to use dynamic allocation, go ahead */
  sq.initialize(input.num_edges * 2); // should be enough ...
  
  ggc::Timer t("sssp");

  int src = 0;
  n_threads = atoi(argv[3]);
  std::vector<std::thread> pool;
  int *marker = new int[n_threads];
  for (int i = 0; i < n_threads; i++) {
    marker[i] = BLACK;
  }

  /* no need to parallelize this */
  sssp_init(input, src);

  t.start();
  
  sq.push(src);

  for (int i = 0; i < n_threads; i++) {
    pool.push_back(std::thread(sssp, input, &sq, marker, i));
  }

  for (auto &th: pool) {
    th.join();
  }
  
  t.stop();

  printf("Total time: %u ms\n", t.duration_ms());

  write_output(input, argv[2]);

  printf("Wrote output '%s'\n", argv[2]);
  return 0;
}
