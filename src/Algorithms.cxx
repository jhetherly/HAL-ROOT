#include <TArrayI.h>

/*
 * This function is designed to help iterate over all unique
 * n-tupled combinations of a collection. What this function 
 * actually returns is an array of indices or NULL if the 
 * end is reached. All memory is managed by the function
 * itself. Upon reaching the last n-tuple the function 
 * returns NULL, indicating the end of the algorithm. For 
 * usage look in the tests folder for 'TestAlgorithms.C'. 
 * */

namespace HAL {

TArrayI* getNextCombination (Int_t size, Int_t n,
                             TArrayI *indices = NULL) {
  if (n > size)
    return NULL;
  if (indices == NULL) {
    TArrayI *result = new TArrayI(n);
    for (Int_t i = 0; i < n; ++i)
      (*result)[i] = i;
    return result;
  }
  if (indices->GetSize() != n)
    return NULL;
  for (Int_t i = 0; i < n - 1; ++i) { // loop through all members except last
    if ((*indices)[i] + 1 == (*indices)[i + 1]) // reset index
      (*indices)[i] = i;
    else {
      ++(*indices)[i];
      return indices;
    }
  }
  if ((*indices)[n - 1] + 1 == size) {
    delete indices;
    return NULL;
  }
  ++(*indices)[n - 1];
  return indices;
}

}
