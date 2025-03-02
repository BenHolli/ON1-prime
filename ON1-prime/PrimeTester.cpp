#include "PrimeTester.h"
#include <iostream>
#include <cmath>
#include <random>
#include <chrono>


/**
* @brief Brute force algorithm to test primality of a number
* O(sqrt(n)) complexity
* @param n Number to test
* @return True if the number is prime, false otherwise
*/
bool PrimeTester::IsPrime(unsigned long n)
{
    if (n <= 1)
        return false;
    if (n <= 3)
        return true;
    if ((n & 0x01) == 0)        // even numbers
        return false;

    unsigned long sqrt_n = (unsigned int)sqrt(n);
    for (unsigned long i = 3; i <= sqrt_n; i += 2)
    {
        if (n % i == 0)
            return false;
    }

    return true;
}   // IsPrime


/**
* @brief Add a number to the collection to be tested
**/
void PrimeTester::AddNumber(unsigned long n)
{
    // In this implementation, adding numbers will be done before the worker
    // threads start, which makes locking the container unnecessary.
	//std::lock_guard<std::mutex> lock(num_mut);

    /**
    * Thoughts on the choice of container:
	* 
    * Using a vector is great if all the numbers are provided before the workers
	* start processing.  If more numbers are added while the workers are processing,
    * enlarging the vector may require reallocation, which would invalidate the
    * iterator marking the next number to be processed.
    * 
    * Using a forward_list allows new numbers to be added easily while the workers
    * are processing and removing others.  However, this requires a separate heap
    * allocation and extra space for a next_pointer for each number in the list,
    * which is memory-inefficient and unkind to your cache.
    **/
	std::pair<unsigned long, bool> num(n, false);
    numbers.push_back(num);

    // If push_back() had to reallocate more space, the old iterator will be invalid
	next_num = numbers.begin();
}   // AddNumber


/**
* @brief Constructor to generate a list of random numbers to test
* Generate 'count' integers from 1 to 'maxval'.
* @param count Number of random numbers to generate
* @param maxval Maximum value for the random numbers
*/
PrimeTester::PrimeTester(unsigned long count, unsigned long maxval)
{
    numbers.reserve(count);
	next_num = numbers.end();

    // Seed the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, maxval);

	// Generate a list of random numbers to test
	for (unsigned long i = 0; i < count; i++)
	{
        AddNumber(distrib(gen));
	}
}   // PrimeTester


void PrimeTester::Process(void)
{
    // Pull a number out of the container, process it, and print the result
    while (true)
	{
        NumIterType num_it;
		{
			std::lock_guard<std::mutex> lock(num_mut);
			if (next_num == numbers.end())
				break;
            num_it = next_num;
			next_num++;
		}

        num_it->second = IsPrime(num_it->first);

#ifdef PRINT_RESULTS
		if (num_it->second)
        {
            std::lock_guard<std::mutex> lock(cout_mut);
            std::cout << num_it->first << " is prime" << std::endl;
        }
        else
        {
            std::lock_guard<std::mutex> lock(cout_mut);
            std::cout << num_it->first << " is not prime" << std::endl;
        }
#endif PRINT_RESULTS
	}
}   // Process

