# ON1-prime

This directory contains the Visual Studio project.

## Goal

Please write a C++ program that reads a list of numbers from a STL collection
and for each number determine if the number is prime.

The prime check should occur in a secondary thread and put the results in a
STL data structure.

Use two worker threads to perform the calculations (one prime number per thread).

## Implementation

This implementation takes as a command line argument the number of integers
to be tested for primality.  It fills a vector with N integers incrementing
from 1 to N.  This vector contains a pair for each value, representing both the
integer being tested and a boolean indicating its tested primality.

Two threads are spawned that will process the entire collection of integers
simultaneously.  Each thread processes the next integer in the collection as
quickly as possible until the entire collection has been processed.

Since the primality test algorithm was not specified, nor was the priority
of code simplicity vs run time, I chose to write a simple, brute force test
with O(sqrt(N)) complexity.  If the particular use case is known, various
algorithms can provide better performance with certain trade-offs.  For
example, Miller-Rabin requires more complex code, but is quite fast and
provides deterministic results for integers as large as 3e12.

## Possible algorithms

[Wikipedia article on Primality Tests](https://en.wikipedia.org/wiki/Primality_test)


