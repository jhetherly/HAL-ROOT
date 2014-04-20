#include <HAL/Algorithms.h>

ClassImp(HAL::GenericData);

namespace HAL
{

/*
 * Generic classes
 * */
internal::ImportTLVAlgo::ImportTLVAlgo (TString name, TString title) : 
  HAL::Algorithm(name, title) {
  fVectorOutput = gaMakeVecLabel(GetName());
  fIndexOutput = gaMakeIndexLabel(GetName());
  fNObjectsOutput = gaMakeNObjectsLabel(GetName());
}

void internal::ImportTLVAlgo::Exec (unsigned n) {
  HAL::AnalysisData *data = GetUserData();

  for (unsigned i = 0; i < n; ++i) {
    TLorentzVector *vec = MakeTLV(i);
    data->SetValue(fVectorOutput, vec, i);
    data->SetValue(fIndexOutput, i, i);
  }
  data->SetValue(fNObjectsOutput, n);
  IncreaseCounter(n);
}

void internal::ImportTLVAlgo::Clear (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  long long n = data->GetInteger(fNObjectsOutput);

  for (long long i = 0; i < n; ++i)
    delete data->GetTObject(fVectorOutput, i);
  data->RemoveAllAssociatedData(GetName());
}

void internal::NthElementAlgo::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  long long n, nreal;
  TString RefInput;
  TString SortedIndexListName;
  TString NObjectsInput = gaMakeNObjectsLabel(fInput);
  TString IndexInput = gaMakeIndexLabel(fInput);
  TString NObjectsOutput = gaMakeNObjectsLabel(GetName());
  TString NameOutput = gaMakeRefNameLabel(GetName());
  TString IndexOutput = gaMakeIndexLabel(GetName());

  if (gaCheckRefName(data, fInput, RefInput)) {
    SortedIndexListName = gaMakeSortedListLabel(RefInput, SortTag());
    fElementName = gaMakeVecLabel(RefInput);
    n = data->GetInteger(NObjectsInput);
    nreal = data->GetInteger(gaMakeNObjectsLabel(RefInput));
  }
  else
    return;

  if (n < fN)
    return;

  // create sorted list for real data
  if (!data->Exists(SortedIndexListName)) {
    long long count = 0;
    std::vector<long long> IndexProxy;

    for (unsigned i = 0; i < nreal; ++i)
      IndexProxy.push_back(i);
    Sort(IndexProxy);
    for (std::vector<long long>::iterator it = IndexProxy.begin(); it != IndexProxy.end(); ++it)
      data->SetValue(SortedIndexListName, *it, count++);
  }

  // loop over the sorted list and find the fN-th ranked member in fInput
  long long location = -1;
  long long count = 0;
  for (long long i = 0; i < nreal; ++i) {
    long long rank_index = data->GetInteger(SortedIndexListName, i);
    // check if this rank_index is in fInput
    for (long long j = 0; j < n; ++j) {
      if (rank_index == data->GetInteger(IndexInput, j)) {
        if (++count == fN) {
          location = rank_index;
          break;
        }
      }
    }
    if (location != -1)
      break;
  }

  data->SetValue(NObjectsOutput, (long long)1);
  data->SetValue(NameOutput, RefInput);
  data->SetValue(IndexOutput, location, 0);
  IncreaseCounter(data->GetInteger(NObjectsOutput));
}

void internal::NthElementAlgo::Clear (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  data->RemoveAllAssociatedData(GetName());
}

void internal::FilterTLVAlgo::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  TString NObjectsInput = gaMakeNObjectsLabel(fInput);
  TString IndexInput = gaMakeIndexLabel(fInput);
  TString NObjectsOutput = gaMakeNObjectsLabel(GetName());
  TString NameOutput = gaMakeRefNameLabel(GetName());
  TString IndexOutput = gaMakeIndexLabel(GetName());
  long long n, count;
  TString RefInput;
  long long *InputIndices;
  TLorentzVector **InputVec;

  if (data->Exists(NObjectsInput)) {
    n = data->GetInteger(NObjectsInput);
    InputVec = new TLorentzVector*[n];
    InputIndices = new long long[n];
  }
  else
    return;

  if (gaCheckRefName(data, fInput, RefInput)) {
    TString RealVec = gaMakeVecLabel(RefInput);
    for (long long i = 0; i < n; ++i) {
      InputIndices[i] = data->GetInteger(IndexInput, i);
      InputVec[i] = (TLorentzVector*)data->GetTObject(RealVec, InputIndices[i]);
    }
  }
  else {
    delete[] InputVec;
    delete[] InputIndices;
    return;
  }

  count = 0;
  for (long long i = 0; i < n; ++i) {
    if (FilterPredicate(InputVec[i])) {
      data->SetValue(IndexOutput, InputIndices[i], count);
      ++count;
    }
  }
  if (count > 0) {
    data->SetValue(NameOutput, RefInput);
    data->SetValue(NObjectsOutput, count);
    IncreaseCounter(data->GetInteger(NObjectsOutput));
  }
  delete[] InputVec;
  delete[] InputIndices;
}

void internal::FilterTLVAlgo::Clear (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  data->RemoveAllAssociatedData(GetName());
}

void internal::FilterRefTLVAlgo::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  TString NObjectsInput = gaMakeNObjectsLabel(fInput);
  TString IndexInput = gaMakeIndexLabel(fInput);
  TString NObjectsOthers = gaMakeNObjectsLabel(fOthers);
  TString IndexOthers = gaMakeIndexLabel(fOthers);
  TString NObjectsOutput = gaMakeNObjectsLabel(GetName());
  TString NameOutput = gaMakeRefNameLabel(GetName());
  TString IndexOutput = gaMakeIndexLabel(GetName());
  long long n, nothers, count;
  TString RefInput, RefOthers;
  long long *InputIndices, *OthersIndices;
  TLorentzVector **InputVec, **OthersVec;

  if (data->Exists(NObjectsInput) && data->Exists(NObjectsOthers)) {
    n = data->GetInteger(NObjectsInput);
    nothers = data->GetInteger(NObjectsOthers);
    InputVec = new TLorentzVector*[n];
    OthersVec = new TLorentzVector*[nothers];
    InputIndices = new long long[n];
    OthersIndices = new long long[nothers];
  }
  else
    return;

  if (n != 1)
    return;

  if (gaCheckRefName(data, fInput, RefInput) &&
      gaCheckRefName(data, fOthers, RefOthers)) {
    TString RealVec = gaMakeVecLabel(RefInput);
    TString RefOthersVec = gaMakeVecLabel(RefOthers);
    InputIndices[0] = data->GetInteger(IndexInput, 0);
    InputVec[0] = (TLorentzVector*)data->GetTObject(RealVec, InputIndices[0]);
    for (long long i = 0; i < nothers; ++i) {
      OthersIndices[i] = data->GetInteger(IndexOthers, i);
      OthersVec[i] = (TLorentzVector*)data->GetTObject(RefOthersVec, OthersIndices[i]);
    }
  }
  else {
    delete[] InputVec;
    delete[] InputIndices;
    delete[] OthersVec;
    delete[] OthersIndices;
    return;
  }

  count = 0;
  for (long long i = 0; i < nothers; ++i) {
    if (FilterPredicate(InputVec[0], OthersVec[i])) {
      data->SetValue(IndexOutput, OthersIndices[i], count);
      ++count;
    }
  }
  if (count > 0) {
    data->SetValue(NameOutput, RefOthers);
    data->SetValue(NObjectsOutput, count);
    IncreaseCounter(data->GetInteger(NObjectsOutput));
  }
  delete[] InputVec;
  delete[] InputIndices;
  delete[] OthersVec;
  delete[] OthersIndices;
}

void internal::FilterRefTLVAlgo::Clear (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  data->RemoveAllAssociatedData(GetName());
}

internal::FilterParentAlgo::FilterParentAlgo (TString name, TString title, 
    TString input, TString logic, long long length, ...) :
  Algorithm(name, title), fExclude(logic.EqualTo("ex", TString::kIgnoreCase)), fInput(input) {

  fRefNames = new const char*[length];
  va_list arguments;  // store the variable list of arguments

  va_start (arguments, length); // initializing arguments to store all values after length
  for (long long i = 0; i < length; ++i)
    fRefNames[i] = va_arg(arguments, const char*);
  va_end(arguments); // cleans up the list
}

void internal::FilterParentAlgo::Exec (Option_t* /*option*/) {
}

void internal::FilterParentAlgo::Clear (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  data->RemoveAllAssociatedData(GetName());
}

//void internal::ParticlesTLVCut::Exec (Option_t* /*option*/) {
//  HAL::AnalysisData *data = GetUserData();
//  long long n;
//  TString NObjectsInput = gaMakeNObjectsLabel(fInput);
//  TString IndexInput = gaMakeIndexLabel(fInput);
//  TString RefInput;
//  TLorentzVector **InputVec;
//
//  if (data->Exists(NObjectsInput)) {
//    n = data->GetInteger(NObjectsInput);
//    InputVec = new TLorentzVector*[n];
//  }
//  else
//    return;
//
//  if (gaCheckRefName(data, fInput, RefInput)) {
//    TString RefVec = gaMakeVecLabel(RefInput);
//    for (long long i = 0; i < n; ++i) {
//      long long inputIndex = data->GetInteger(IndexInput, i);
//      InputVec[i] = (TLorentzVector*)data->GetTObject(RefVec, inputIndex);
//    }
//  }
//  else
//    return;
//
//  for (long long i = 0; i < n; ++i) {
//    if (!CutPredicate(InputVec[i])) {
//      Abort();
//      return;
//    }
//  }
//  Passed();
//}

void internal::ParticlesTLVStore::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  HAL::AnalysisTreeWriter *output = GetUserOutput();
  long long n;
  TString NObjectsInput = gaMakeNObjectsLabel(fInput);
  TString IndexInput = gaMakeIndexLabel(fInput);
  TString RefInput;
  TLorentzVector **InputVec;

  if (data->Exists(NObjectsInput)) {
    n = data->GetInteger(NObjectsInput);
    InputVec = new TLorentzVector*[n];
  }
  else
    return;

  if (gaCheckRefName(data, fInput, RefInput)) {
    TString RefVec = gaMakeVecLabel(RefInput);
    for (long long i = 0; i < n; ++i) {
      long long inputIndex = data->GetInteger(IndexInput, i);
      InputVec[i] = (TLorentzVector*)data->GetTObject(RefVec, inputIndex);
    }
  }
  else
    return;

  if (fMany) {
    for (long long i = 0; i < n; ++i)
      output->SetValue(fBranchName, StoreValue(InputVec[i]), i);
  }
  else
    output->SetValue(fBranchName, StoreValue(InputVec[0]));
}








/*
 * Actual classes
 * */

/*
 * Importing Algorithms
 * */

Algorithms::ImportTLV::ImportTLV (TString name, TString title, unsigned n) : 
  ImportTLVAlgo(name, title), fN(n), fIsCart(false), fIsE(false), fIsM(false) {}

void Algorithms::ImportTLV::Init (Option_t* /*option*/) {
  HAL::AnalysisTreeReader *tr = GetRawData();
  TString CartEntriesName = TString::Format("%s:x1", GetName().Data());
  TString MassEntriesName = TString::Format("%s:m", GetName().Data());
  TString EnergyEntriesName = TString::Format("%s:e", GetName().Data());

  if (tr->CheckBranchMapNickname(CartEntriesName))
    fIsCart = true;
  else if (tr->CheckBranchMapNickname(EnergyEntriesName))
    fIsE = true;
  else if (tr->CheckBranchMapNickname(MassEntriesName))
    fIsM = true;
  if (!fIsCart && !fIsE && !fIsM)
    HAL::HALException(GetName().Prepend("Couldn't determine how to import data: ").Data());

  fCartX0 = TString::Format("%s:x0", GetName().Data());
  fCartX1 = TString::Format("%s:x1", GetName().Data());
  fCartX2 = TString::Format("%s:x2", GetName().Data());
  fCartX3 = TString::Format("%s:x3", GetName().Data());
  fPt = TString::Format("%s:pt", GetName().Data());
  fEta = TString::Format("%s:eta", GetName().Data());
  fPhi = TString::Format("%s:phi", GetName().Data());
  fM = TString::Format("%s:m", GetName().Data());
  fE = TString::Format("%s:e", GetName().Data());
  fNEntriesName = TString::Format("%s:nentries", GetName().Data());
  fCartEntriesName = TString::Format("%s:x1", GetName().Data());
  fPtEntriesName = TString::Format("%s:pt", GetName().Data());
}

void Algorithms::ImportTLV::Exec (Option_t* /*option*/) {
  HAL::AnalysisTreeReader *tr = GetRawData();
  long long n = fN;

  // determine number of elements to read in
  if (n == 0) {
    if (tr->CheckBranchMapNickname(fNEntriesName))
      n = tr->GetInteger(fNEntriesName);
    else if (fIsCart)
      n = tr->GetDim(fCartEntriesName);
    else if (fIsE || fIsM)
      n = tr->GetDim(fPtEntriesName);
  }
  // call actual Exec algo
  ImportTLVAlgo::Exec(n);
}

TLorentzVector* Algorithms::ImportTLV::MakeTLV (unsigned i) {
  HAL::AnalysisTreeReader *tr = GetRawData();

  if (fIsCart) {
    long double x0 = tr->GetDecimal(fCartX0, i),
                x1 = tr->GetDecimal(fCartX1, i),
                x2 = tr->GetDecimal(fCartX2, i),
                x3 = tr->GetDecimal(fCartX3, i);
    return new TLorentzVector(x1, x2, x3, x0);
  }
  else if (fIsE) {
    long double e = tr->GetDecimal(fE, i),
                pT = tr->GetDecimal(fPt, i),
                eta = tr->GetDecimal(fEta, i),
                phi = tr->GetDecimal(fPhi, i);
    return HAL::makeTLVFromPtEtaPhiE(pT, eta, phi, e);
  }
  else if (fIsM) {
    long double m = tr->GetDecimal(fM, i),
                pT = tr->GetDecimal(fPt, i),
                eta = tr->GetDecimal(fEta, i),
                phi = tr->GetDecimal(fPhi, i);
    return HAL::makeTLVFromPtEtaPhiM(pT, eta, phi, m);
  }
  throw HAL::HALException("Couldn't identify type in ImportTLV");
}

/*
 * Reconstruction Algorithms
 * */

Algorithms::VecAddReco::VecAddReco (TString name, TString title, long long length, ...) :
    Algorithm(name, title), fLength(length) {
  fParentNames = new const char*[fLength];
  va_list arguments;  // store the variable list of arguments

  va_start (arguments, length); // initializing arguments to store all values after length
  for (long long i = 0; i < fLength; ++i)
    fParentNames[i] = va_arg(arguments, const char*);
  va_end(arguments); // cleans up the list
}

Algorithms::VecAddReco::~VecAddReco() {
  delete[] fParentNames;
}

void Algorithms::VecAddReco::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  TString *RefInputs = new TString[fLength];
  long long *ParentNObjects = new long long[fLength];
  long long **ParentIndices2 = new long long*[fLength];
  TLorentzVector ***ParentVecs2 = new TLorentzVector**[fLength];
  std::set<TString> UniqueRefInput;
  std::map<TString, std::set<std::set<long long> > > UniqueTuples;
  long long UniqueTuplesMaxLength = 0;
  std::map<TString, long long> UniqueTuplesLengths;
  std::vector<TString> UniqueTuplesLabels;
  std::deque<std::vector<long long> > UniqueTuplesValues;
  std::vector<TLorentzVector*> UniqueTuplesVectors;
  // scalars
  TString NObjectsOutput = gaMakeNObjectsLabel(GetName());
  TString NParentsOutput = gaMakeNParentsLabel(GetName());
  // 1D arrays
  TString VectorOutput = gaMakeVecLabel(GetName());
  TString IndexOutput = gaMakeIndexLabel(GetName());
  TString ParentNamesOutput = gaMakeParentRefNameLabel(GetName());
  // 2D arrays
  TString ParentIndicesOutput = gaMakeParentIndexLabel(GetName());

  // Modify to look at parents too and assume that parents already form unique tuples
  for (long long i = 0; i < fLength; ++i) {
    TString pname(fParentNames[i]);
    if (gaCheckRefName(data, pname, RefInputs[i])) {
      UniqueRefInput.insert(RefInputs[i]);
      ParentNObjects[i] = data->GetInteger(gaMakeNObjectsLabel(fParentNames[i]).Data());
      ParentIndices2[i] = new long long[ParentNObjects[i]];
      ParentVecs2[i] = new TLorentzVector*[ParentNObjects[i]];
      for (long long j = 0; j < ParentNObjects[i]; ++j) {
        ParentIndices2[i][j] = data->GetInteger(gaMakeIndexLabel(fParentNames[i]).Data(), j);
        ParentVecs2[i][j] = (TLorentzVector*)data->GetTObject(gaMakeVecLabel(RefInputs[i]), ParentIndices2[i][j]);
      }
    }
    else
      return;
  }

  // remove any overlap
  // loop over unique real names
  for (std::set<TString>::iterator name = UniqueRefInput.begin(); name != UniqueRefInput.end(); ++name) {
    for (long long i = 0; i < fLength; ++i) {
      if (RefInputs[i] == *name) {
        if (UniqueTuples.count(*name) == 0) {
          std::set<std::set<long long> > index_tuples;
          for (long long j = 0; j < ParentNObjects[i]; ++j) {
            std::set<long long> index_tuple;
            index_tuple.insert(ParentIndices2[i][j]);
            index_tuples.insert(index_tuple);
          }
          UniqueTuples[*name] = index_tuples;
        }
        else {
          std::set<std::set<long long> > index_tuples;
          for (std::set<std::set<long long> >::iterator tuple = UniqueTuples[*name].begin();
               tuple != UniqueTuples[*name].end(); ++tuple) {
            for (long long j = 0; j < ParentNObjects[i]; ++j) {
              std::set<long long> index_tuple = *tuple;
              index_tuple.insert(ParentIndices2[i][j]);
              index_tuples.insert(index_tuple);
            }
          }
          UniqueTuples[*name] = index_tuples;
        }
      }
    }
  }
  // find largest size of set for each type, then remove all that are smaller than this
  for (std::map<TString, std::set<std::set<long long> > >::iterator mapit = UniqueTuples.begin();
       mapit != UniqueTuples.end(); ++mapit) {
    unsigned max_size = 0;
    for (std::set<std::set<long long> >::iterator setit = mapit->second.begin();
         setit != mapit->second.end(); ++setit) {
      if (setit->size() > max_size)
        max_size = setit->size();
    }
    UniqueTuplesMaxLength += max_size;
    UniqueTuplesLengths[mapit->first] = max_size;
    for (std::set<std::set<long long> >::iterator setit = mapit->second.begin();
         setit != mapit->second.end(); ) {
      if (setit->size() < max_size)
        mapit->second.erase(setit++);
      else
        ++setit;
    }
  }
  // print out UniqueTuples
  //for (std::map<TString, std::set<std::set<long long> > >::iterator mapit = UniqueTuples.begin();
  //     mapit != UniqueTuples.end(); ++mapit) {
  //  std::cout << mapit->first << std::endl;
  //  for (std::set<std::set<long long> >::iterator setit = mapit->second.begin();
  //       setit != mapit->second.end(); ++setit) {
  //    for (std::set<long long>::iterator value = setit->begin(); value != setit->end(); ++value) {
  //      std::cout << *value << "   ";
  //    }
  //    std::cout << std::endl;
  //  }
  //}

  // merge tuples to form array of indices
  for (std::map<TString, std::set<std::set<long long> > >::iterator mapit = UniqueTuples.begin();
       mapit != UniqueTuples.end(); ++mapit) {
    for (long long i = 0; i < UniqueTuplesLengths[mapit->first]; ++i)
      UniqueTuplesLabels.push_back(mapit->first);
    if (UniqueTuplesValues.size() == 0) {
      for (std::set<std::set<long long> >::iterator setit = mapit->second.begin();
          setit != mapit->second.end(); ++setit) {
        std::vector<long long> indices;
        for (std::set<long long>::iterator value = setit->begin(); value != setit->end(); ++value) {
          indices.push_back(*value);
        }
        UniqueTuplesValues.push_back(indices);
      }
    }
    else {
      for (std::deque<std::vector<long long> >::iterator tuple = UniqueTuplesValues.begin();
           tuple != UniqueTuplesValues.end(); ++tuple) {
        for (unsigned i = 0; i < mapit->second.size(); ++i) {
          UniqueTuplesValues.insert(tuple++, *tuple);
        }
      }
      for (std::deque<std::vector<long long> >::iterator tuple = UniqueTuplesValues.begin();
           tuple != UniqueTuplesValues.end(); ++tuple) {
        for (std::set<std::set<long long> >::iterator setit = mapit->second.begin();
            setit != mapit->second.end(); ++setit) {
          for (std::set<long long>::iterator value = setit->begin(); value != setit->end(); ++value) {
            tuple->push_back(*value);
            ++tuple;
          }
        }
      }
    }
  }
  if (UniqueTuplesMaxLength != fLength)
    return;
  // print out final result
  //for (std::vector<TString>::iterator name = UniqueTuplesLabels.begin(); name != UniqueTuplesLabels.end(); ++name) {
  //  std::cout << *name << "\t\t";
  //}
  //std::cout << std::endl;
  //for (std::deque<std::vector<long long> >::iterator tuple = UniqueTuplesValues.begin();
  //     tuple != UniqueTuplesValues.end(); ++tuple) {
  //  for (std::vector<long long>::iterator value = tuple->begin(); value != tuple->end(); ++value) {
  //    std::cout << *value << "\t\t";
  //  }
  //  std::cout << std::endl;
  //}


  for (unsigned i = 0; i < UniqueTuplesValues.size(); ++i) {
    for (unsigned j = 0; j < UniqueTuplesValues[i].size(); ++j) { // loop over all indices
      if (j == 0)
        UniqueTuplesVectors.push_back(new TLorentzVector(*((TLorentzVector*)data->GetTObject(gaMakeVecLabel(UniqueTuplesLabels[j]), UniqueTuplesValues[i][j]))));
      else
        UniqueTuplesVectors[i]->operator+=(*((TLorentzVector*)data->GetTObject(gaMakeVecLabel(UniqueTuplesLabels[j]), UniqueTuplesValues[i][j])));
    }
  }
  //for (std::vector<TLorentzVector*>::iterator vec = UniqueTuplesVectors.begin();
  //     vec != UniqueTuplesVectors.end(); ++vec) {
  //  std::cout << (*vec)->Pt() << "    ";
  //}
  //std::cout << std::endl;

  data->SetValue(NParentsOutput, fLength);
  data->SetValue(NObjectsOutput, UniqueTuplesVectors.size());
  for (unsigned i = 0; i < UniqueTuplesVectors.size(); ++i) {
    data->SetValue(VectorOutput, UniqueTuplesVectors[i], i);
    data->SetValue(IndexOutput, i, i);
  }
  for (unsigned i = 0; i < UniqueTuplesLabels.size(); ++i)
    data->SetValue(ParentNamesOutput, UniqueTuplesLabels[i], i);
  for (unsigned i = 0; i < UniqueTuplesValues.size(); ++i) {
    for (unsigned j = 0; j < UniqueTuplesValues[i].size(); ++j) {
      data->SetValue(ParentIndicesOutput, UniqueTuplesValues[i][j], i, j);
    }
  }
  IncreaseCounter(data->GetInteger(NObjectsOutput));
  delete[] RefInputs;
  delete[] ParentNObjects;
  for (int i = 0; i < fLength; ++i) {
    delete[] ParentIndices2[i];
    delete[] ParentVecs2[i];
  }
  delete[] ParentIndices2;
  delete[] ParentVecs2;
}

void Algorithms::VecAddReco::Clear (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  TString VectorOutput = gaMakeVecLabel(GetName());
  TString NObjectsOutput = gaMakeNObjectsLabel(GetName());

  if (data->Exists(VectorOutput)) {
    for (long long i = 0; i < data->GetInteger(NObjectsOutput); ++i)
      delete data->GetTObject(VectorOutput, i);
  }
  data->RemoveAllAssociatedData(GetName());
}

Algorithms::RankSelectionTLV::RankSelectionTLV (TString name, TString title, 
    TString input, unsigned rank, TString property, TString end) : 
  NthElementAlgo(name, title, input, rank), fPt(false), fM(false), fE(false),
    fEt(false), fP3(false), fHigh(false), fLow(false),
    fTLVProperty(property), fEnd(end) {
  if (fTLVProperty.EqualTo("pt", TString::kIgnoreCase))
    fPt = true;
  else if (fTLVProperty.EqualTo("m", TString::kIgnoreCase))
    fM = true;
  else if (fTLVProperty.EqualTo("e", TString::kIgnoreCase))
    fE = true;
  else if (fTLVProperty.EqualTo("et", TString::kIgnoreCase))
    fEt = true;
  else if (fTLVProperty.EqualTo("p3", TString::kIgnoreCase))
    fP3 = true;

  if (fEnd.EqualTo("high", TString::kIgnoreCase))
    fHigh = true;
  else if (fEnd.EqualTo("low", TString::kIgnoreCase))
    fLow = true;
}

TString Algorithms::RankSelectionTLV::SortTag () {
  if (fPt)
    return "4v_pt";
  if (fM)
    return "4v_m";
  if (fE)
    return "4v_e";
  if (fEt)
    return "4v_et";
  if (fP3)
    return "4v_p3";
  throw HAL::HALException(GetName().Prepend("Couldn't determine sorting type: "));
}

bool Algorithms::RankSelectionTLV::operator() (long long lhs, long long rhs) {
  AnalysisData *data = GetUserData();
  TLorentzVector *lhs_vec = (TLorentzVector*)data->GetTObject(fElementName, lhs);
  TLorentzVector *rhs_vec = (TLorentzVector*)data->GetTObject(fElementName, rhs);

  if (fHigh) {
    if (fPt)
      return (lhs_vec->Pt() > rhs_vec->Pt());
    if (fM)
      return (lhs_vec->M() > rhs_vec->M());
    if (fE)
      return (lhs_vec->E() > rhs_vec->E());
    if (fEt)
      return (lhs_vec->Et() > rhs_vec->Et());
    if (fP3)
      return (lhs_vec->P() > rhs_vec->P());
  }
  else if (fLow) {
    if (fPt)
      return (lhs_vec->Pt() < rhs_vec->Pt());
    if (fM)
      return (lhs_vec->M() < rhs_vec->M());
    if (fE)
      return (lhs_vec->E() < rhs_vec->E());
    if (fEt)
      return (lhs_vec->Et() < rhs_vec->Et());
    if (fP3)
      return (lhs_vec->P() < rhs_vec->P());
  }
  throw HAL::HALException(GetName().Prepend("Couldn't determine sorting information: "));
}

void Algorithms::RankSelectionTLV::Sort (std::vector<long long> &ip) {
  std::stable_sort(ip.begin(), ip.end(), *this);
}

Algorithms::SelectTLV::SelectTLV (TString name, TString title, TString input, TString property, 
    double value, TString end) : 
  FilterTLVAlgo(name, title, input), 
  fPt(false), fM(false), fE(false), fEt(false), fP3(false), fEta(false), 
  fPhi(false), fHigh(false), fLow(false), fWindow(false),
  fTLVProperty(property), fEnd(end) {

  if (end.EqualTo("low", TString::kIgnoreCase)) {
    fLow = true;
    fLowLimit = value;
  }
  else if (end.EqualTo("high", TString::kIgnoreCase)) {
    fHigh = true;
    fHighLimit = value;
  }
  Setup();
}

Algorithms::SelectTLV::SelectTLV (TString name, TString title, TString input, TString property, 
    double low, double high) : 
  FilterTLVAlgo(name, title, input), 
  fHighLimit(high), fLowLimit(low), 
  fPt(false), fM(false), fE(false), fEt(false), fP3(false), fEta(false), 
  fPhi(false), fHigh(false), fLow(false), fWindow(true),
  fTLVProperty(property), fEnd("window") {
  Setup();
}

void Algorithms::SelectTLV::Setup () {
  if (fTLVProperty.EqualTo("pt", TString::kIgnoreCase))
    fPt = true;
  if (fTLVProperty.EqualTo("m", TString::kIgnoreCase))
    fM = true;
  if (fTLVProperty.EqualTo("e", TString::kIgnoreCase))
    fE = true;
  if (fTLVProperty.EqualTo("et", TString::kIgnoreCase))
    fEt = true;
  if (fTLVProperty.EqualTo("p3", TString::kIgnoreCase))
    fP3 = true;
  if (fTLVProperty.EqualTo("eta", TString::kIgnoreCase))
    fEta = true;
  if (fTLVProperty.EqualTo("phi", TString::kIgnoreCase))
    fPhi = true;
}

bool Algorithms::SelectTLV::FilterPredicate(TLorentzVector *vec) {
  if (!fWindow) {
    if (fLow) {
      if (fPt)
        return (vec->Pt() >= fLowLimit);
      else if (fM)
        return (vec->M() >= fLowLimit);
      else if (fE)
        return (vec->E() >= fLowLimit);
      else if (fEt)
        return (vec->Et() >= fLowLimit);
      else if (fP3)
        return (vec->P() >= fLowLimit);
      else if (fEta)
        return (vec->Eta() >= fLowLimit);
      else if (fPhi)
        return (vec->Phi() >= fLowLimit);
    }
    else if (fHigh) {
      if (fPt)
        return (vec->Pt() <= fHighLimit);
      else if (fM)
        return (vec->M() <= fHighLimit);
      else if (fE)
        return (vec->E() <= fHighLimit);
      else if (fEt)
        return (vec->Et() <= fHighLimit);
      else if (fP3)
        return (vec->P() <= fHighLimit);
      else if (fEta)
        return (vec->Eta() <= fHighLimit);
      else if (fPhi)
        return (vec->Phi() <= fHighLimit);
    }
  }
  else { // window cut
    if (fPt)
      return (vec->Pt() <= fHighLimit && vec->Pt() >= fLowLimit);
    else if (fM)
      return (vec->M() <= fHighLimit && vec->M() >= fLowLimit);
    else if (fE)
      return (vec->E() <= fHighLimit && vec->E() >= fLowLimit);
    else if (fEt)
      return (vec->Et() <= fHighLimit && vec->Et() >= fLowLimit);
    else if (fP3)
      return (vec->P() <= fHighLimit && vec->P() >= fLowLimit);
    else if (fEta)
      return (vec->Eta() <= fHighLimit && vec->Eta() >= fLowLimit);
    else if (fPhi)
      return (vec->Phi() <= fHighLimit && vec->Phi() >= fLowLimit);
  }
  throw HAL::HALException(GetName().Prepend("Couldn't determine how to filter: "));
}

Algorithms::SelectDeltaTLV::SelectDeltaTLV (TString name, TString title, TString input, TString others, 
    double value, TString topo, TString type) : 
  FilterRefTLVAlgo(name, title, input, others), fIn(false), fOut(false), 
  fWindow(false), fDeltaR(false), fDeltaPhi(false) {

  if (topo.EqualTo("in", TString::kIgnoreCase)) {
    fIn = true;
    fHighLimit = value;
  }
  else if (topo.EqualTo("out", TString::kIgnoreCase)) {
    fOut = true;
    fLowLimit = value;
  }
  if (type.EqualTo("r", TString::kIgnoreCase)) 
    fDeltaR = true;
  else if (type.EqualTo("phi", TString::kIgnoreCase)) 
    fDeltaPhi = true;
}

Algorithms::SelectDeltaTLV::SelectDeltaTLV (TString name, TString title, TString input, TString others, 
    double low, double high, TString type) :
  FilterRefTLVAlgo(name, title, input, others), fHighLimit(high), fLowLimit(low), fIn(false), fOut(false), 
  fWindow(true), fDeltaR(false), fDeltaPhi(false) {

  if (type.EqualTo("r", TString::kIgnoreCase)) 
    fDeltaR = true;
  else if (type.EqualTo("phi", TString::kIgnoreCase)) 
    fDeltaPhi = true;
}

bool Algorithms::SelectDeltaTLV::FilterPredicate (TLorentzVector *ref, TLorentzVector *vec) {
  if (fDeltaR) {
    double dR = ref->DeltaR(*vec);
    if (fIn) {
      if (dR <= fHighLimit)
        return true;
    }
    else if (fOut) {
      if (dR >= fLowLimit)
        return true;
    }
    else if (fWindow) {
      if (dR >= fLowLimit && dR <= fHighLimit)
        return true;
    }
  }
  else if (fDeltaPhi) {
    double dPhi = ref->DeltaPhi(*vec);
    if (fIn) {
      if (dPhi <= fHighLimit)
        return true;
    }
    else if (fOut) {
      if (dPhi >= fLowLimit)
        return true;
    }
    else if (fWindow) {
      if (dPhi >= fLowLimit && dPhi <= fHighLimit)
        return true;
    }
  }
  return false;
}

/*
 * Cutting Algorithms
 * */

//Algorithms::CutTLV::CutTLV (TString name, TString title, TString input, TString property, 
//    double value, TString end) : 
//  ParticlesTLVCut(name, title, input), 
//  fPt(false), fM(false), fE(false), fEt(false), fP3(false), fEta(false), 
//  fPhi(false), fHigh(false), fLow(false), fWindow(false),
//  fTLVProperty(property), fEnd(end) {
//
//  if (end.EqualTo("low", TString::kIgnoreCase)) {
//    fLow = true;
//    fLowLimit = value;
//  }
//  else if (end.EqualTo("high", TString::kIgnoreCase)) {
//    fHigh = true;
//    fHighLimit = value;
//  }
//  Setup();
//}
//
//Algorithms::CutTLV::CutTLV (TString name, TString title, TString input, TString property, 
//    double low, double high) : 
//  ParticlesTLVCut(name, title, input), 
//  fHighLimit(high), fLowLimit(low), 
//  fPt(false), fM(false), fE(false), fEt(false), fP3(false), fEta(false), 
//  fPhi(false), fHigh(false), fLow(false), fWindow(true),
//  fTLVProperty(property), fEnd("window") {
//  Setup();
//}
//
//void Algorithms::CutTLV::Setup () {
//  if (fTLVProperty.EqualTo("pt", TString::kIgnoreCase))
//    fPt = true;
//  if (fTLVProperty.EqualTo("m", TString::kIgnoreCase))
//    fM = true;
//  if (fTLVProperty.EqualTo("e", TString::kIgnoreCase))
//    fE = true;
//  if (fTLVProperty.EqualTo("et", TString::kIgnoreCase))
//    fEt = true;
//  if (fTLVProperty.EqualTo("p3", TString::kIgnoreCase))
//    fP3 = true;
//  if (fTLVProperty.EqualTo("eta", TString::kIgnoreCase))
//    fEta = true;
//  if (fTLVProperty.EqualTo("phi", TString::kIgnoreCase))
//    fPhi = true;
//}
//
//bool Algorithms::CutTLV::CutPredicate(TLorentzVector *vec) {
//  if (!fWindow) {
//    if (fLow) {
//      if (fPt)
//        return (vec->Pt() >= fLowLimit);
//      else if (fM)
//        return (vec->M() >= fLowLimit);
//      else if (fE)
//        return (vec->E() >= fLowLimit);
//      else if (fEt)
//        return (vec->Et() >= fLowLimit);
//      else if (fP3)
//        return (vec->P() >= fLowLimit);
//      else if (fEta)
//        return (vec->Eta() >= fLowLimit);
//      else if (fPhi)
//        return (vec->Phi() >= fLowLimit);
//    }
//    else if (fHigh) {
//      if (fPt)
//        return (vec->Pt() <= fHighLimit);
//      else if (fM)
//        return (vec->M() <= fHighLimit);
//      else if (fE)
//        return (vec->E() <= fHighLimit);
//      else if (fEt)
//        return (vec->Et() <= fHighLimit);
//      else if (fP3)
//        return (vec->P() <= fHighLimit);
//      else if (fEta)
//        return (vec->Eta() <= fHighLimit);
//      else if (fPhi)
//        return (vec->Phi() <= fHighLimit);
//    }
//  }
//  else { // window cut
//    if (fPt)
//      return (vec->Pt() <= fHighLimit && vec->Pt() >= fLowLimit);
//    else if (fM)
//      return (vec->M() <= fHighLimit && vec->M() >= fLowLimit);
//    else if (fE)
//      return (vec->E() <= fHighLimit && vec->E() >= fLowLimit);
//    else if (fEt)
//      return (vec->Et() <= fHighLimit && vec->Et() >= fLowLimit);
//    else if (fP3)
//      return (vec->P() <= fHighLimit && vec->P() >= fLowLimit);
//    else if (fEta)
//      return (vec->Eta() <= fHighLimit && vec->Eta() >= fLowLimit);
//    else if (fPhi)
//      return (vec->Phi() <= fHighLimit && vec->Phi() >= fLowLimit);
//  }
//  throw HAL::HALException(GetName().Prepend("Couldn't determine how to cut: "));
//}

Algorithms::CutNObjects::CutNObjects (TString name, TString title, TString logic, 
    long long n, long long length, ...) :
  CutAlgorithm(name, title), fAnd(false), fOr(false), fLength(length), fN(n) {
  fParticleNames = new const char*[fLength];
  va_list arguments;  // store the variable list of arguments

  if (logic.EqualTo("and", TString::kIgnoreCase))
    fAnd = true;
  else if (logic.EqualTo("or", TString::kIgnoreCase))
    fOr = true;
  va_start (arguments, length); // initializing arguments to store all values after length
  for (long long i = 0; i < fLength; ++i)
    fParticleNames[i] = va_arg(arguments, const char*);
  va_end(arguments); // cleans up the list
}

void Algorithms::CutNObjects::Exec (Option_t* /*option*/) {
  AnalysisData *data = GetUserData();

  if (fAnd) {
    for (long long i = 0; i < fLength; ++i) {
      TString NObjects = gaMakeNObjectsLabel(fParticleNames[i]);
      if (!data->Exists(NObjects)) {
        Abort();
        return;
      }
      else if (data->GetInteger(NObjects) < fN) {
        Abort();
        return;
      }
    }

    Passed();
  }
  else if (fOr) {
    for (long long i = 0; i < fLength; ++i) {
      TString NObjects = gaMakeNObjectsLabel(fParticleNames[i]);
      if (data->Exists(NObjects) && data->GetInteger(NObjects) >= fN) {
        Passed();
        return;
      }
    }

    Abort();
  }
}

/*
 * Exporting Algorithms
 * */

Algorithms::StoreTLV::StoreTLV (TString name, TString title, TString input, 
    TString property, TString bname, TString num) :
  ParticlesTLVStore(name, title, input, bname, num) {

  if (property.EqualTo("pt", TString::kIgnoreCase))
    fPt = true;
  else if (property.EqualTo("m", TString::kIgnoreCase))
    fM = true;
  else if (property.EqualTo("e", TString::kIgnoreCase))
    fE = true;
  else if (property.EqualTo("et", TString::kIgnoreCase))
    fEt = true;
  else if (property.EqualTo("p3", TString::kIgnoreCase))
    fP3 = true;
  else if (property.EqualTo("eta", TString::kIgnoreCase))
    fEta = true;
  else if (property.EqualTo("phi", TString::kIgnoreCase))
    fPhi = true;
}

double Algorithms::StoreTLV::StoreValue (TLorentzVector *vec) {
  if (fPt)
    return vec->Pt();
  if (fM)
    return vec->M();
  if (fE)
    return vec->E();
  if (fEt)
    return vec->Et();
  if (fP3)
    return vec->P();
  if (fEta)
    return vec->Eta();
  if (fPhi)
    return vec->Phi();
  throw HAL::HALException(GetName().Prepend("Couldn't determine what to store: "));
}

} /* HAL */ 
