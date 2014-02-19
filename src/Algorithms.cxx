#include <HAL/Algorithms.h>

namespace HAL
{

TArrayI* getNextCombination (Int_t size, Int_t n,
                             TArrayI *indices) {
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

} /* HAL */ 
