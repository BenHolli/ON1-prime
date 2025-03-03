/*****
* ON1-prime
*
* Goal:
*
* Please write a C++ program that reads a list of numbers from a STL
* collection and for each number determine if the number is prime.
* The prime check should occur in a secondary thread and put the results
* in a STL data structure. Use two worker threads to perform the
* calculations (one prime number per thread).
*
* Implementation:
*
* This implementation takes as a command line argument the number of integers
* to be tested for primality.  It fills a vector with N integers incrementing
* from 1 to N.  This vector contains a pair for each value, representing the
* integer being tested and a boolean indicating its tested primality.
*
* Two threads are spawned that will process the entire collection of integers
* simultaneously.  Each thread processes the next integer in the collection as
* quickly as possible until the entire collection has been processed.
*
* Since the primality test algorithm was not specified, nor was the priority
* of code simplicity vs run time, I chose to write a simple, brute force test
* with O(sqrt(N)) complexity.  If the particular use case is known, various
* algorithms can provide better performance with certain trade-offs.  For
* example, Miller-Rabin requires more complex code, but is quite fast and
* provides deterministic results for integers as large as 3e12.
*/

#include <string>
#include <iostream>
#include <thread>
#include <climits>
#include "PrimeTester.h"

// Use for creating a set of all integers from 1 to <count>
#define USE_LINEAR 1


/**
 * @brief RTFM
 * @param argv0 The name of the executable being run
 */
void Usage(const char *argv0)
{
#if defined(USE_LINEAR)
	std::cerr << "Usage: " << argv0 << " <count>" << std::endl;
	std::cerr << "  <count> must be a positive integer less than "
			  << LONG_MAX << std::endl;
#else
	std::cerr << "Usage: " << argv0 << " <count> <maxval>" << std::endl;
	std::cerr << "  <count> and <maxval> must be positive integers less than "
			  << LONG_MAX << std::endl;
#endif
	exit(1);
}


int main(int argc, char *argv[])
{
#if defined(USE_LINEAR)
	// This creates a deterministic set of numbers for time testing

	if (argc != 2)
		Usage(argv[0]);

	// stoi() throws exceptions for strings representing out of range values
	unsigned long count = 0;
	try {
		long long icount = std::stoi(argv[1]);
		if (icount > 0 && icount <= LONG_MAX)
			count = icount;
	}
	catch (std::exception& e) {
		Usage(argv[0]);
	}
	if (count < 1)
		Usage(argv[0]);

	PrimeTester tester(count);

#else
	// This creates a set of random numbers

	if (argc != 3)
		Usage(argv[0]);

	// strtoi() throws exceptions for strings representing out of range values
	unsigned long count = 0;
	unsigned long maxval = 0;
	try {
		long long icount = std::stoi(argv[1]);
		long long imaxval = std::stoi(argv[2]);
		if (icount > 0 && icount <= LONG_MAX)
			count = icount;
		if (imaxval > 0 && imaxval <= LONG_MAX)
			maxval = imaxval;
	}
	catch (std::exception& e) {
		Usage(argv[0]);
	}
	if (count < 1 || maxval < 1)
		Usage(argv[0]);

	PrimeTester tester(count, maxval);
#endif

	int num_threads = 2;	// Or std::thread::hardware_concurrency();
	std::cout << "Testing " << count << " numbers for primality across "
			  << num_threads << " threads..." << std::endl;

	// Launch several threads running tester.Process()
	std::vector<std::thread> workers;
	for (int i = 0; i < num_threads; i++) {
		std::thread t(&PrimeTester::Process, &tester);
		workers.push_back(std::move(t));
	}

	// Wait until the workers have tested all the numbers
	for (auto& t : workers) {
		t.join();
	}

	// Double check the threaded results
	//tester.Verify();

	return 0;
}
