/*****
* ON1-prime
*
* Goal:
* Please write a C++ program that reads a list of numbers from a STL
* collection and for each number determine if the number is prime.
* The prime check should occur in a secondary thread and put the results
* in a STL data structure. Use two worker threads to perform the
* calculations (one prime number per thread).
*/

#include <string>
#include <iostream>
#include <thread>
#include "PrimeTester.h"


void Usage(const char *argv0)
{
	std::cerr << "Usage: " << argv0 << " <count> <maxval>" << std::endl;
	std::cerr << "  <count> and <maxval> must be positive integers less than " << LONG_MAX << std::endl;
	exit(1);
}


int main(int argc, char *argv[])
{
#ifdef USE_STDIN
	PrimeTester tester;

	// Read the numbers to be tested from stdin.
	// This part can be replaced to suit the use case.
	std::cout << "Enter numbers to check, separated by spaces, followed by a 0 to commence testing: " << std::endl;
	unsigned long count = 0;
	while (true) {
		unsigned long num;
		std::cin >> num;
		if (num == 0) {
			break;
		}
		tester.AddNumber(num);
		count++;
	}

#else

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
	std::cout << "Testing " << count << " numbers for primality..." << std::endl;

	// Launch several threads running tester.Process()
	std::vector<std::thread> workers;
	for (int i = 0; i < 2; i++) {		// Or std::thread::hardware_concurrency()
		std::thread t(&PrimeTester::Process, &tester);
		workers.push_back(std::move(t));
	}

	// Wait until the workers have tested all the numbers
	for (auto& t : workers) {
		t.join();
	}

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
