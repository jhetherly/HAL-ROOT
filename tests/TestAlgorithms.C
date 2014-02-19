TestAlgorithms(Int_t size = 4, Int_t n = 2)                                                                                                  
{                                                                                                                       
  gSystem->Load("libHAL");

  // start testing getNextCombination
  std::cout << "Testing getNextCombination(" << size << ", " << n << ")" << std::endl;
  TArrayI *values = new TArrayI(size);
  TArrayI *indices = NULL;
  TRandom3 rnd(593);

  std::cout << "\nInitial array:" << std::endl;
  for (Int_t i = 0; i < values->GetSize(); ++i) {
    (*values)[i] = rnd.Integer(100);
    std::cout << (*values)[i] << "   ";
  }
  std::cout << std::endl << "\n" << n << "-tupled combinations:\n\n";
  // This gives the initial combination
  indices = HAL::getNextCombination(size, n);
  // Loop as long as there are valid indicies
  while (indices != NULL) {
    std::cout << "Indices:   ";
    for (Int_t i = 0; i < indices->GetSize(); ++i) {
      std::cout << (*indices)[i] << "   ";
    }
    std::cout << std::endl << "Values:   ";
    for (Int_t i = 0; i < indices->GetSize(); ++i) {
      Int_t ii = (*indices)[i];
      std::cout << (*values)[ii] << "   ";
    }
    std::cout << std::endl << std::endl;
    // This gets the next set of indicies or NULL if no more
    indices = HAL::getNextCombination(size, n, indices);
  }
  // no need to delete indices, getNextCombination does this when it gets to the end
  delete values;
  // finished testing getNextCombination
}
