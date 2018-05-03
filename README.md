# mpi-Codes
implementing sorting algorithms in mpi

I have implemented bitonic and odd-even sort as part of a project in Distributed systems course in IIIT Hyderabad.

The result shows bitonic sort to be better than odd-even sort, because of O(n) communication cost in odd-even sort.

To run the code:

$ mpic++ odd_even.cpp 

$ mpirun -n 4 ./a.out 32

where 4 are your number of processes and 32 is total size of your array, you want to sort.
