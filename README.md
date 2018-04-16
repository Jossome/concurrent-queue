# concurrent-queue

- `test.sh, testpara.sh, timing.sh, get.py, alltest.sh` are used for running experiments and storing results into `.csv` files.
- use `make` to make all binaries
- run sssp binaries using:
```
./sssp_xxx inputfile outputfile n_threads
```
- test sssp binaries using:
```
./testpara.sh ./sssp_xxx inputfile sample_outputfile n_threads
```