/**
 * Code for the PrimeTester class.
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

#include "PrimeTester.h"
#include <iostream>
#include <cmath>
#include <random>
#include <chrono>
#include <thread>


/**
* @brief Brute force algorithm to test primality of a number
* O(sqrt(n)) complexity
* @param n Number to test
* @return True if the number is prime, false if not
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
* @param n The number to add
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
* @brief Constructor to generate a linear list of numbers to test
* Generate 'count' integers from 1 to 'count'.
* @param count How many linear numbers to generate
*/
PrimeTester::PrimeTester(unsigned long count)
{
    numbers.reserve(count);

	// Generate a list of random numbers to test
	for (unsigned long i = 1; i <= count; i++)
	{
        AddNumber(i);
	}

	next_num = numbers.begin();
}   // PrimeTester


/**
* @brief Constructor to generate a list of random numbers to test
* Generate 'count' integers from 1 to 'maxval'.
* @param count Number of random numbers to generate
* @param maxval Maximum value for the random numbers
*/
PrimeTester::PrimeTester(unsigned long count, unsigned long maxval)
{
    numbers.reserve(count);

    // Seed the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, maxval);

	// Generate a list of random numbers to test
	for (unsigned long i = 0; i < count; i++)
	{
        AddNumber(distrib(gen));
	}

	next_num = numbers.begin();
}   // PrimeTester


/**
 * @brief Test the numbers in the container for primality
 * This function is thread-safe, and will repeatedly process the next number
 * in the container until all the numbers have been processed.
 * This method will continue to run until the entire container has been exhausted.
 */
void PrimeTester::Process(void)
{
    unsigned long count = 0;
    std::thread::id myID = std::this_thread::get_id();

    // Pull a number out of the container, process it, and print the result
    while (true)
	{
        // Make note of the next number to be processed, then advance the
        // pointer so the following number will be retrieved next time.
        NumIterType num_it;
		{
			std::lock_guard<std::mutex> lock(num_mut);
			if (next_num == numbers.end())
				break;
            num_it = next_num;
			next_num++;
		}

        num_it->second = IsPrime(num_it->first);
        count++;
	}

    // Bookkeeping for the curious
    std::lock_guard<std::mutex> lock(cout_mut);
    std::cout << "Thread " << myID << " processed " << count << " numbers" << std::endl;
}   // Process


/**
 * @brief Compare threaded results against single-threaded results
 * This function is not thread-safe, because the threading functionality
 * is part of what we're verifying herein.  It should only be called after
 * all Process() threads have completed their work.
 */
bool PrimeTester::Verify(void)
{
    std::cout << "Verifying results..." << std::endl;
    unsigned long failed = 0;
    unsigned long primes = 0;
    for (auto num : numbers)
    {
        bool isprime = IsPrime(num.first);
        if (num.second)
            primes++;
        if (isprime != num.second)
        {
            std::cout << "Number " << num.first << " failed.  Found " << num.second
                      << ", should be " << isprime << std::endl;
            failed++;
        }
    }

    if (failed == 0)
        std::cout << "Passed test" << std::endl;
    else
        std::cout << "Failed " << failed << " tests" << std::endl;
    std::cout << "Found " << primes << " primes" << std::endl;

    return (failed == 0);
}

