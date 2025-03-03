/**
 * Header for the PrimeTester class.
 *
 * The class will hold a collection of numbers to be tested for primality,
 * optionally using the constructor to fill it with linear or random values.
 *
 * The Process() method will test the primality of every value in the
 * collection in a thread-safe way so that multiple threads may run
 * Process() concurrently.
 *
 * The Verify() method will make a half-hearted effort to confirm that
 * the primality results are correct.  Verify() is not thread-safe.
 */

#pragma once

#include <vector>
#include <mutex>


class PrimeTester
{
public:
	PrimeTester() { next_num = numbers.end(); };
	PrimeTester(unsigned long maxval);
	PrimeTester(unsigned long count, unsigned long maxval);

	void AddNumber(unsigned long n);
	void Process(void);
	bool Verify(void);

private:
	bool IsPrime(unsigned long n);

	std::mutex cout_mut;	// For dumping results to stdout
	std::mutex num_mut;		// For accessing the collection of numbers

	// The list of numbers are stored as a pair containing both
	// the number to be processed and it's resulting primality.
	// The result is left here in this private data structure pending
	// further info about how the caller wants to retrieve it.
	typedef std::pair<unsigned long, bool> NumType;
	typedef std::vector<NumType>::iterator NumIterType;

	// The collection of numbers to be tested
	std::vector<NumType> numbers;

	// Pointer to the next number to be processed.  Each thread will
	// make a copy of this iterator for its own use and then advance
	// the iterator so that the next thread gets the next number.
	NumIterType next_num;
};

