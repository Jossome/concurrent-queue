all: sssp_serial harness

sssp_serial: sssp_serial.cpp Timer.h simplegraph.h serialqueue.h
	g++ -g -O3 $< -o $@

harness: harness.cpp blocking_queue.h lockfree_queue.h
	g++ -g -O3 $< -o $@ -pthread -std=c++11 -latomic

clean:
	rm -f sssp_serial harness
