#ifndef HAL_ALGORITHMS
#define HAL_ALGORITHMS

#include <TArrayI.h>
#include <HAL/Common.h>

/*
 * This function is designed to help iterate over all unique
 * n-tupled combinations of a collection. What this function 
 * actually returns is an array of indices or NULL if the 
 * end is reached. All memory is managed by the function
 * itself. Upon reaching the last n-tuple the function 
 * returns NULL, indicating the end of the algorithm. For 
 * usage look in the tests folder for 'TestAlgorithms.C'. 
 * */

namespace HAL
{

TArrayI* getNextCombination (Int_t size, Int_t n, TArrayI *indices = NULL);

} /* HAL */ 

#endif /* end of include guard: HAL_ALGORITHMS */

