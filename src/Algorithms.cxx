#include <HAL/Algorithms.h>

namespace HAL
{

/*
 * Generic functions
 * */
bool internal::determineAccessProtocol(HAL::AnalysisData *data, 
                                       TString &RawInput, 
                                       TString &RealInput) {
  TString VectorInput = TString::Format("%s:4-vec", RawInput.Data());   // either this
  TString NameInput = TString::Format("%s:ref_name", RawInput.Data());  // or this must exist

  if (data->Exists(VectorInput.Data())) {
    RealInput = RawInput;
    return true;
  }
  else if (data->Exists(NameInput.Data())) {
    RealInput = TString(data->GetString(NameInput.Data()).c_str());
    return true;
  }
  return false;
}

/*
 * Generic classes
 * */
void internal::ImportTLVAlgo::Exec (unsigned n) {
  HAL::AnalysisData *data = GetUserData();
  TString VectorOutput = TString::Format("%s:4-vec", GetName().Data());
  TString IndexOutput = TString::Format("%s:index", GetName().Data());
  TString NObjectsOutput = TString::Format("%s:nobjects", GetName().Data());

  for (unsigned i = 0; i < n; ++i) {
    TLorentzVector *vec = MakeTLV(i);
    data->SetValue(VectorOutput.Data(), vec, i);
    data->SetValue(IndexOutput.Data(), i, i);
  }
  data->SetValue(NObjectsOutput.Data(), n);
}

void internal::ImportTLVAlgo::Clear (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  unsigned n = data->GetInteger(TString::Format("%s:nobjects", GetName().Data()).Data());
  TString VectorOutput = TString::Format("%s:4-vec", GetName().Data());

  for (unsigned i = 0; i < n; ++i)
    delete data->GetTObject(VectorOutput.Data());
  data->RemoveAllAssociatedData(GetName().Data());
}

void internal::NthElementAlgo::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  unsigned n, nreal;
  TString RealInput;
  TString SortedIndexListName;
  TString NObjectsInput = TString::Format("%s:nobjects", fInput.Data());
  TString IndexInput = TString::Format("%s:index", fInput.Data());
  TString NObjectsOutput = TString::Format("%s:nobjects", GetName().Data());
  TString NameOutput = TString::Format("%s:ref_name", GetName().Data());
  TString IndexOutput = TString::Format("%s:index", GetName().Data());

  if (determineAccessProtocol(data, fInput, RealInput)) {
    TString NObjectsReal = TString::Format("%s:nobjects", RealInput.Data());
    SortedIndexListName = TString::Format("%s:sorted_%s", RealInput.Data(), SortTag().Data());
    fElementName = TString::Format("%s:4-vec", RealInput.Data());
    n = data->GetInteger(NObjectsInput.Data());
    nreal = data->GetInteger(NObjectsReal.Data());
  }
  else
    return;

  if (n < fN)
    return;

  // create sorted list for real data
  if (!data->Exists(SortedIndexListName.Data())) {
    long long count = 0;
    std::vector<long long> IndexProxy;

    for (unsigned i = 0; i < nreal; ++i)
      IndexProxy.push_back(i);
    Sort(IndexProxy);
    for (std::vector<long long>::iterator it = IndexProxy.begin(); it != IndexProxy.end(); ++it)
      data->SetValue(SortedIndexListName.Data(), *it, count++);
  }

  // loop over the sorted list and find the fN-th ranked member in fInput
  long long location = -1;
  long long count = 0;
  for (unsigned i = 0; i < nreal; ++i) {
    long long rank_index = data->GetInteger(SortedIndexListName.Data(), i);
    // check if this rank_index is in fInput
    for (unsigned j = 0; j < n; ++j) {
      if (rank_index == data->GetInteger(IndexInput.Data(), j)) {
        if (++count == fN) {
          location = rank_index;
          break;
        }
      }
    }
    if (location != -1)
      break;
  }

  data->SetValue(NObjectsOutput.Data(), (long long)1);
  data->SetValue(NameOutput.Data(), RealInput);
  data->SetValue(IndexOutput.Data(), location, 0);
}

void internal::NthElementAlgo::Clear (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  data->RemoveAllAssociatedData(GetName().Data());
}

void internal::FilterTLVAlgo::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  TString NObjectsInput = TString::Format("%s:nobjects", fInput.Data());
  TString IndexInput = TString::Format("%s:index", fInput.Data());
  TString NObjectsOutput = TString::Format("%s:nobjects", GetName().Data());
  TString NameOutput = TString::Format("%s:ref_name", GetName().Data());
  TString IndexOutput = TString::Format("%s:index", GetName().Data());
  long long n, count;
  TString RealInput;
  long long *InputIndices;
  TLorentzVector **InputVec;

  if (data->Exists(NObjectsInput.Data())) {
    n = data->GetInteger(NObjectsInput.Data());
    InputVec = new TLorentzVector*[n];
    InputIndices = new long long[n];
  }
  else
    return;

  if (internal::determineAccessProtocol(data, fInput, RealInput)) {
    TString RealVec = TString::Format("%s:4-vec", RealInput.Data());
    for (long long i = 0; i < n; ++i) {
      InputIndices[i] = data->GetInteger(IndexInput.Data(), i);
      InputVec[i] = (TLorentzVector*)data->GetTObject(RealVec.Data(), InputIndices[i]);
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
      data->SetValue(IndexOutput.Data(), InputIndices[i], count);
      ++count;
    }
  }
  if (count > 0) {
    data->SetValue(NameOutput.Data(), RealInput);
    data->SetValue(NObjectsOutput.Data(), count);
  }
  delete[] InputVec;
  delete[] InputIndices;
}

void internal::FilterTLVAlgo::Clear (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  data->RemoveAllAssociatedData(GetName().Data());
}

void internal::ParticlesTLVCut::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  long long n;
  TString NObjectsInput = TString::Format("%s:nobjects", fInput.Data());
  TString IndexInput = TString::Format("%s:index", fInput.Data());
  TString RealInput;
  TLorentzVector **InputVec;

  if (data->Exists(NObjectsInput.Data())) {
    n = data->GetInteger(NObjectsInput.Data());
    InputVec = new TLorentzVector*[n];
  }
  else
    return;

  if (internal::determineAccessProtocol(data, fInput, RealInput)) {
    TString RealVec = TString::Format("%s:4-vec", RealInput.Data());
    for (long long i = 0; i < n; ++i) {
      long long inputIndex = data->GetInteger(IndexInput.Data(), i);
      InputVec[i] = (TLorentzVector*)data->GetTObject(RealVec.Data(), inputIndex);
    }
  }
  else
    return;

  for (long long i = 0; i < n; ++i) {
    if (!CutPredicate(InputVec[i])) {
      Abort();
      return;
    }
  }
  Passed();
}

void internal::SingleParticleTLVStore::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  HAL::AnalysisTreeWriter *output = GetUserOutput();
  long long n;
  TString NObjectsInput = TString::Format("%s:nobjects", fInput.Data());
  TString IndexInput = TString::Format("%s:index", fInput.Data());
  TString RealInput;
  TLorentzVector **InputVec;

  if (data->Exists(NObjectsInput.Data())) {
    n = data->GetInteger(NObjectsInput.Data());
    InputVec = new TLorentzVector*[n];
  }
  else
    return;

  if (internal::determineAccessProtocol(data, fInput, RealInput)) {
    TString RealVec = TString::Format("%s:4-vec", RealInput.Data());
    for (long long i = 0; i < n; ++i) {
      long long inputIndex = data->GetInteger(IndexInput.Data(), i);
      InputVec[i] = (TLorentzVector*)data->GetTObject(RealVec.Data(), inputIndex);
    }
  }
  else
    return;

  output->SetValue(fBranchName.Data(), StoreValue(InputVec[0]));
}

void internal::ParticlesTLVStore::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  HAL::AnalysisTreeWriter *output = GetUserOutput();
  long long n;
  TString NObjectsInput = TString::Format("%s:nobjects", fInput.Data());
  TString IndexInput = TString::Format("%s:index", fInput.Data());
  TString RealInput;
  TLorentzVector **InputVec;

  if (data->Exists(NObjectsInput.Data())) {
    n = data->GetInteger(NObjectsInput.Data());
    InputVec = new TLorentzVector*[n];
  }
  else
    return;

  if (internal::determineAccessProtocol(data, fInput, RealInput)) {
    TString RealVec = TString::Format("%s:4-vec", RealInput.Data());
    for (long long i = 0; i < n; ++i) {
      long long inputIndex = data->GetInteger(IndexInput.Data(), i);
      InputVec[i] = (TLorentzVector*)data->GetTObject(RealVec.Data(), inputIndex);
    }
  }
  else
    return;

  for (long long i = 0; i < n; ++i)
    output->SetValue(fBranchName.Data(), StoreValue(InputVec[i]), i);
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
}

void Algorithms::ImportTLV::Exec (Option_t* /*option*/) {
  HAL::AnalysisTreeReader *tr = GetRawData();
  TString NEntriesName = TString::Format("%s:nentries", GetName().Data());
  TString CartEntriesName = TString::Format("%s:x1", GetName().Data());
  TString PtEntriesName = TString::Format("%s:pt", GetName().Data());
  long long n = fN;

  // determine number of elements to read in
  if (n == 0) {
    if (tr->CheckBranchMapNickname(NEntriesName))
      n = tr->GetInteger(NEntriesName.Data());
    else if (fIsCart)
      n = tr->GetDim(CartEntriesName.Data());
    else if (fIsE || fIsM)
      n = tr->GetDim(PtEntriesName.Data());
  }
  // call actual Exec algo
  ImportTLVAlgo::Exec(n);
}

TLorentzVector* Algorithms::ImportTLV::MakeTLV (unsigned i) {
  HAL::AnalysisTreeReader *tr = GetRawData();

  if (fIsCart) {
    long double x0 = tr->GetDecimal(TString::Format("%s:x0", GetName().Data()), i),
                x1 = tr->GetDecimal(TString::Format("%s:x1", GetName().Data()), i),
                x2 = tr->GetDecimal(TString::Format("%s:x2", GetName().Data()), i),
                x3 = tr->GetDecimal(TString::Format("%s:x3", GetName().Data()), i);
    return new TLorentzVector(x1, x2, x3, x0);
  }
  else if (fIsE) {
    long double e = tr->GetDecimal(TString::Format("%s:e", GetName().Data()), i),
                pT = tr->GetDecimal(TString::Format("%s:pt", GetName().Data()), i),
                eta = tr->GetDecimal(TString::Format("%s:eta", GetName().Data()), i),
                phi = tr->GetDecimal(TString::Format("%s:phi", GetName().Data()), i);
    return HAL::makeTLVFromPtEtaPhiE(pT, eta, phi, e);
  }
  else if (fIsM) {
    long double m = tr->GetDecimal(TString::Format("%s:m", GetName().Data()), i),
                pT = tr->GetDecimal(TString::Format("%s:pT", GetName().Data()), i),
                eta = tr->GetDecimal(TString::Format("%s:eta", GetName().Data()), i),
                phi = tr->GetDecimal(TString::Format("%s:phi", GetName().Data()), i);
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
  TString *RealInputs = new TString[fLength];
  long long *ParentNObjects = new long long[fLength];
  long long **ParentIndices2 = new long long*[fLength];
  TLorentzVector ***ParentVecs2 = new TLorentzVector**[fLength];
  std::set<TString> UniqueRealInput;
  std::map<TString, std::set<std::set<long long> > > UniqueTuples;
  long long UniqueTuplesMaxLength = 0;
  std::map<TString, long long> UniqueTuplesLengths;
  std::vector<TString> UniqueTuplesLabels;
  std::deque<std::vector<long long> > UniqueTuplesValues;
  std::vector<TLorentzVector*> UniqueTuplesVectors;
  // scalars
  TString NObjectsOutput = TString::Format("%s:nobjects", GetName().Data());
  TString NParentsOutput = TString::Format("%s:nparents", GetName().Data());
  // 1D arrays
  TString VectorOutput = TString::Format("%s:4-vec", GetName().Data());
  TString IndexOutput = TString::Format("%s:index", GetName().Data());
  TString ParentNamesOutput = TString::Format("%s:parent_ref_name", GetName().Data());
  TString ParentIndicesOutput = TString::Format("%s:parent_index", GetName().Data());

  for (long long i = 0; i < fLength; ++i) {
    TString pname(fParentNames[i]);
    if (internal::determineAccessProtocol(data, pname, RealInputs[i])) {
      UniqueRealInput.insert(RealInputs[i]);
      ParentNObjects[i] = data->GetInteger(TString::Format("%s:nobjects", fParentNames[i]).Data());
      ParentIndices2[i] = new long long[ParentNObjects[i]];
      ParentVecs2[i] = new TLorentzVector*[ParentNObjects[i]];
      for (long long j = 0; j < ParentNObjects[i]; ++j) {
        ParentIndices2[i][j] = data->GetInteger(TString::Format("%s:index", fParentNames[i]).Data(), j);
        ParentVecs2[i][j] = (TLorentzVector*)data->GetTObject(TString::Format("%s:4-vec", RealInputs[i].Data()).Data(), ParentIndices2[i][j]);
      }
    }
    else
      return;
  }

  // remove any overlap
  // loop over unique real names
  for (std::set<TString>::iterator name = UniqueRealInput.begin(); name != UniqueRealInput.end(); ++name) {
    for (long long i = 0; i < fLength; ++i) {
      if (RealInputs[i] == *name) {
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
        UniqueTuplesVectors.push_back(new TLorentzVector(*((TLorentzVector*)data->GetTObject(TString::Format("%s:4-vec", UniqueTuplesLabels[j].Data()).Data(), UniqueTuplesValues[i][j]))));
      else
        UniqueTuplesVectors[i]->operator+=(*((TLorentzVector*)data->GetTObject(TString::Format("%s:4-vec", UniqueTuplesLabels[j].Data()).Data(), UniqueTuplesValues[i][j])));
    }
  }
  //for (std::vector<TLorentzVector*>::iterator vec = UniqueTuplesVectors.begin();
  //     vec != UniqueTuplesVectors.end(); ++vec) {
  //  std::cout << (*vec)->Pt() << "    ";
  //}
  //std::cout << std::endl;

  data->SetValue(NParentsOutput.Data(), fLength);
  data->SetValue(NObjectsOutput.Data(), UniqueTuplesVectors.size());
  for (unsigned i = 0; i < UniqueTuplesVectors.size(); ++i) {
    data->SetValue(VectorOutput.Data(), UniqueTuplesVectors[i], i);
    data->SetValue(IndexOutput.Data(), i, i);
  }
  for (unsigned i = 0; i < UniqueTuplesLabels.size(); ++i)
    data->SetValue(ParentNamesOutput.Data(), UniqueTuplesLabels[i], i);
  for (unsigned i = 0; i < UniqueTuplesValues.size(); ++i) {
    for (unsigned j = 0; j < UniqueTuplesValues[i].size(); ++j) {
      data->SetValue(ParentIndicesOutput.Data(), UniqueTuplesValues[i][j], i, j);
    }
  }
  delete[] RealInputs;
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
  TString VectorOutput = TString::Format("%s:4-vec", GetName().Data());
  TString NObjectsOutput = TString::Format("%s:nobjects", GetName().Data());

  if (data->Exists(VectorOutput.Data())) {
    for (long long i = 0; i < data->GetInteger(NObjectsOutput.Data()); ++i)
      delete data->GetTObject(VectorOutput.Data(), i);
  }
  data->RemoveAllAssociatedData(GetName().Data());
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
  TLorentzVector *lhs_vec = (TLorentzVector*)data->GetTObject(fElementName.Data(), lhs);
  TLorentzVector *rhs_vec = (TLorentzVector*)data->GetTObject(fElementName.Data(), rhs);

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

/*
 * Actual classes
 * */

/*
 * Importing Algorithms
 * */

/*
 * Filtering Algorithms
 * */
//TString FA0000::SortTag () {
//  return "4v_pt";
//}
//
//bool FA0000::operator() (long long lhs, long long rhs) {
//  AnalysisData *data = GetUserData();
//  TLorentzVector *lhs_vec = (TLorentzVector*)data->GetTObject(fElementName.Data(), lhs);
//  TLorentzVector *rhs_vec = (TLorentzVector*)data->GetTObject(fElementName.Data(), rhs);
//
//  // decending order (greatest to least)
//  return (lhs_vec->Pt() > rhs_vec->Pt());
//}
//
//void FA0000::Sort (std::vector<long long> &ip) {
//  std::stable_sort(ip.begin(), ip.end(), *this);
//}

bool FA0100::FilterPredicate(TLorentzVector *vec) {
  return (vec->Pt() >= fValue);
}

bool FA0101::FilterPredicate(TLorentzVector *vec) {
  return (vec->Eta() >= fValue);
}

bool FA0102::FilterPredicate(TLorentzVector *vec) {
  return (vec->Phi() >= fValue);
}

bool FA0103::FilterPredicate(TLorentzVector *vec) {
  return (vec->M() >= fValue);
}

bool FA0104::FilterPredicate(TLorentzVector *vec) {
  return (vec->E() >= fValue);
}

bool FA0110::FilterPredicate(TLorentzVector *vec) {
  return (vec->Pt() <= fValue);
}

bool FA0111::FilterPredicate(TLorentzVector *vec) {
  return (vec->Eta() <= fValue);
}

bool FA0112::FilterPredicate(TLorentzVector *vec) {
  return (vec->Phi() <= fValue);
}

bool FA0113::FilterPredicate(TLorentzVector *vec) {
  return (vec->M() <= fValue);
}

bool FA0114::FilterPredicate(TLorentzVector *vec) {
  return (vec->E() <= fValue);
}

bool FA0120::FilterPredicate(TLorentzVector *vec) {
  return (vec->Pt() <= fHigh && vec->Pt() >= fLow);
}

bool FA0121::FilterPredicate(TLorentzVector *vec) {
  return (vec->Eta() <= fHigh && vec->Eta() >= fLow);
}

bool FA0122::FilterPredicate(TLorentzVector *vec) {
  return (vec->Phi() <= fHigh && vec->Phi() >= fLow);
}

bool FA0123::FilterPredicate(TLorentzVector *vec) {
  return (vec->M() <= fHigh && vec->M() >= fLow);
}

bool FA0124::FilterPredicate(TLorentzVector *vec) {
  return (vec->E() <= fHigh && vec->E() >= fLow);
}
/*
 * Cutting Algorithms
 * */
bool CA0000::CutPredicate (TLorentzVector *vec) {
  return (vec->Pt() >= fCutValue);
}

bool CA0003::CutPredicate (TLorentzVector *vec) {
  return (vec->M() >= fCutValue);
}

CA0100::CA0100 (TString name, TString title, long long n, long long length, ...) :
  CutAlgorithm(name, title), fLength(length), fN(n) {
  fParticleNames = new const char*[fLength];
  va_list arguments;  // store the variable list of arguments

  va_start (arguments, length); // initializing arguments to store all values after length
  for (long long i = 0; i < fLength; ++i)
    fParticleNames[i] = va_arg(arguments, const char*);
  va_end(arguments); // cleans up the list
}

void CA0100::Exec (Option_t* /*option*/) {
  AnalysisData *data = GetUserData();

  for (long long i = 0; i < fLength; ++i) {
    TString NObjects = TString::Format("%s:nobjects", fParticleNames[i]);
    if (!data->Exists(NObjects.Data())) {
      Abort();
      return;
    }
    else if (data->GetInteger(NObjects.Data()) < fN) {
      Abort();
      return;
    }
  }

  Passed();
}

CA0101::CA0101 (TString name, TString title, long long n, long long length, ...) :
  CutAlgorithm(name, title), fLength(length), fN(n) {
  fParticleNames = new const char*[fLength];
  va_list arguments;  // store the variable list of arguments

  va_start (arguments, length); // initializing arguments to store all values after length
  for (long long i = 0; i < fLength; ++i)
    fParticleNames[i] = va_arg(arguments, const char*);
  va_end(arguments); // cleans up the list
}

void CA0101::Exec (Option_t* /*option*/) {
  AnalysisData *data = GetUserData();

  for (long long i = 0; i < fLength; ++i) {
    TString NObjects = TString::Format("%s:nobjects", fParticleNames[i]);
    if (data->Exists(NObjects.Data()) && data->GetInteger(NObjects.Data()) >= fN) {
      Passed();
      return;
    }
  }

  Abort();
}

/*
 * Exporting Algorithms
 * */
double EA0000::StoreValue (TLorentzVector *vec) {
  return vec->Pt();
}

double EA0001::StoreValue (TLorentzVector *vec) {
  return vec->Eta();
}

double EA0002::StoreValue (TLorentzVector *vec) {
  return vec->Phi();
}

double EA0003::StoreValue (TLorentzVector *vec) {
  return vec->M();
}

double EA0004::StoreValue (TLorentzVector *vec) {
  return vec->E();
}

double EA0010::StoreValue (TLorentzVector *vec) {
  return vec->Pt();
}

double EA0011::StoreValue (TLorentzVector *vec) {
  return vec->Eta();
}

double EA0012::StoreValue (TLorentzVector *vec) {
  return vec->Phi();
}

double EA0013::StoreValue (TLorentzVector *vec) {
  return vec->M();
}

double EA0014::StoreValue (TLorentzVector *vec) {
  return vec->E();
}

} /* HAL */ 
