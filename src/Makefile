all: sssp_serial sssp_blocking sssp_lockfree harness

sssp_serial: sssp_serial.cpp Timer.h simplegraph.h serialqueue.h
	g++ -g -O3 $< -o $@

sssp_blocking: sssp_blocking.cpp Timer.h simplegraph.h blocking_queue.h
	g++ -g -O3 $< -o $@ -pthread -std=c++11 -latomic

sssp_lockfree: sssp_lockfree.cpp Timer.h simplegraph.h lockfree_queue.h
	g++ -g -O3 $< -o $@ -pthread -std=c++11 -latomic

harness: harness.cpp blocking_queue.h lockfree_queue.h
	g++ -g -O3 $< -o $@ -pthread -std=c++11 -latomic

clean:
	rm -f sssp_serial sssp_blocking sssp_lockfree harness
