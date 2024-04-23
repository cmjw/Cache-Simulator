Cache Simulator
Elizabeth Luu, Charlotte White
CS 378

-----------------------

To Run All Traces:
$ bash run.sh

This runs each trace, for associativity values of 2, 4, and 8.


To Run:
$ make
$ ./cache_simulator <trace>

To run with minimal output other than stats:
$ ./cache_simulator <trace> -n

To set associativity:
$ ./cache_simulator <trace> -n -a <associativity>

Input:
Trace file in .din Dinero 3 format.
