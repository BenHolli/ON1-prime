#pragma once

#include <vector>
#include <mutex>


class PrimeTester
{
public:
	PrimeTester() { next_num = numbers.end(); };
	PrimeTester(unsigned long count, unsigned long maxval);

	void AddNumber(unsigned long n);
	void Process(void);

private:
	bool IsPrime(unsigned long n);

#ifdef PRINT_RESULTS
	std::mutex cout_mut;	// For dumping results to stdout
#endif
	std::mutex num_mut;		// For accessing the collection of numbers

	// The list of numbers and a pointer to the next one to be processed
	typedef std::pair<unsigned long, bool> NumType;
	typedef std::vector<std::pair<unsigned long, bool>>::iterator NumIterType;
	std::vector<NumType> numbers;
	std::vector<NumType>::iterator next_num;
};

