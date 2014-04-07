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
  unsigned n;
  TString RealInput;
  TString SortedIndexListName;
  TString NObjectsOutput = TString::Format("%s:nobjects", GetName().Data());
  TString NameOutput = TString::Format("%s:ref_name", GetName().Data());
  TString IndexOutput = TString::Format("%s:index", GetName().Data());

  if (determineAccessProtocol(data, fInput, RealInput)) {
    fElementName = TString::Format("%s:4-vec", RealInput.Data());
    n = data->GetInteger(TString::Format("%s:nobjects", RealInput.Data()).Data());
    SortedIndexListName = TString::Format("%s:sorted_%s", RealInput.Data(), SortTag().Data());
  }
  else
    return;

  if (n < fN)
    return;

  if (!data->Exists(SortedIndexListName.Data())) {
    long long count = 0;
    std::vector<long long> IndexProxy;

    for (unsigned i = 0; i < n; ++i)
      IndexProxy.push_back(i);
    Sort(IndexProxy);
    for (std::vector<long long>::iterator it = IndexProxy.begin(); it != IndexProxy.end(); ++it)
      data->SetValue(SortedIndexListName.Data(), *it, count++);
  }

  long long location = data->GetInteger(SortedIndexListName.Data(), fN - 1);
  data->SetValue(NObjectsOutput.Data(), (long long)1);
  data->SetValue(NameOutput.Data(), fInput);
  data->SetValue(IndexOutput.Data(), location, 0);
}

void internal::NthElementAlgo::Clear (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();

  data->RemoveAllAssociatedData(GetName().Data());
}

void internal::ParticlesTLVCut::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  long long n;
  TString *RealInput;
  TLorentzVector **InputVec;

  if (data->Exists(TString::Format("%s:nobjects", fInput.Data()).Data())) {
    n = data->GetInteger(TString::Format("%s:nobjects", fInput.Data()).Data());
    RealInput = new TString[n];
    InputVec = new TLorentzVector*[n];
  }
  else
    return;

  for (long long i = 0; i < n; ++i) {
    if (internal::determineAccessProtocol(data, fInput, RealInput[i])) {
      long long InputIndex = data->GetInteger(TString::Format("%s:index", fInput.Data()).Data(), i);
      InputVec[i] = (TLorentzVector*)data->GetTObject(TString::Format("%s:4-vec", RealInput[i].Data()).Data(), InputIndex);
    }
    else
      return;
  }

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
  TString *RealInput;
  TLorentzVector **InputVec;

  if (data->Exists(TString::Format("%s:nobjects", fInput.Data()).Data())) {
    n = data->GetInteger(TString::Format("%s:nobjects", fInput.Data()).Data());
    RealInput = new TString[n];
    InputVec = new TLorentzVector*[n];
  }
  else
    return;

  for (long long i = 0; i < n; ++i) {
    if (internal::determineAccessProtocol(data, fInput, RealInput[i])) {
      long long InputIndex = data->GetInteger(TString::Format("%s:index", fInput.Data()).Data(), i);
      InputVec[i] = (TLorentzVector*)data->GetTObject(TString::Format("%s:4-vec", RealInput[i].Data()).Data(), InputIndex);
    }
    else
      return;
  }

  output->SetValue(fBranchName.Data(), StoreValue(InputVec[0]));
}

void internal::ParticlesTLVStore::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  HAL::AnalysisTreeWriter *output = GetUserOutput();
  long long n;
  TString *RealInput;
  TLorentzVector **InputVec;

  if (data->Exists(TString::Format("%s:nobjects", fInput.Data()).Data())) {
    n = data->GetInteger(TString::Format("%s:nobjects", fInput.Data()).Data());
    RealInput = new TString[n];
    InputVec = new TLorentzVector*[n];
  }
  else
    return;

  for (long long i = 0; i < n; ++i) {
    if (internal::determineAccessProtocol(data, fInput, RealInput[i])) {
      long long InputIndex = data->GetInteger(TString::Format("%s:index", fInput.Data()).Data(), i);
      InputVec[i] = (TLorentzVector*)data->GetTObject(TString::Format("%s:4-vec", RealInput[i].Data()).Data(), InputIndex);
    }
    else
      return;
  }

  for (long long i = 0; i < n; ++i)
    output->SetValue(fBranchName.Data(), StoreValue(InputVec[i]), i);
}


/*
 * Actual classes
 * */

/*
 * Importing Algorithms
 * */
void IA0000::Exec (Option_t* /*option*/) {
  AnalysisTreeReader *tr = GetRawData();
  unsigned n = tr->GetInteger(TString::Format("%s:nentries", GetName().Data()));

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0000::MakeTLV (unsigned i) {
  AnalysisTreeReader *tr = GetRawData();

  long double x0 = tr->GetDecimal(TString::Format("%s:x0", GetName().Data()), i),
              x1 = tr->GetDecimal(TString::Format("%s:x1", GetName().Data()), i),
              x2 = tr->GetDecimal(TString::Format("%s:x2", GetName().Data()), i),
              x3 = tr->GetDecimal(TString::Format("%s:x3", GetName().Data()), i);
  return new TLorentzVector(x1, x2, x3, x0);
}

void IA0001::Exec (Option_t* /*option*/) {
  AnalysisTreeReader *tr = GetRawData();
  unsigned n = tr->GetDim(TString::Format("%s:x1", GetName().Data()));

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0001::MakeTLV (unsigned i) {
  AnalysisTreeReader *tr = GetRawData();

  long double x0 = tr->GetDecimal(TString::Format("%s:x0", GetName().Data()), i),
              x1 = tr->GetDecimal(TString::Format("%s:x1", GetName().Data()), i),
              x2 = tr->GetDecimal(TString::Format("%s:x2", GetName().Data()), i),
              x3 = tr->GetDecimal(TString::Format("%s:x3", GetName().Data()), i);
  return new TLorentzVector(x1, x2, x3, x0);
}

void IA0002::Exec (Option_t* /*option*/) {
  unsigned n = fNEntries;

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0002::MakeTLV (unsigned i) {
  AnalysisTreeReader *tr = GetRawData();

  long double x0 = tr->GetDecimal(TString::Format("%s:x0", GetName().Data()), i),
              x1 = tr->GetDecimal(TString::Format("%s:x1", GetName().Data()), i),
              x2 = tr->GetDecimal(TString::Format("%s:x2", GetName().Data()), i),
              x3 = tr->GetDecimal(TString::Format("%s:x3", GetName().Data()), i);
  return new TLorentzVector(x1, x2, x3, x0);
}

void IA0010::Exec (Option_t* /*option*/) {
  AnalysisTreeReader *tr = GetRawData();
  unsigned n = tr->GetInteger(TString::Format("%s:nentries", GetName().Data()));

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0010::MakeTLV (unsigned i) {
  AnalysisTreeReader *tr = GetRawData();

  long double e = tr->GetDecimal(TString::Format("%s:e", GetName().Data()), i),
              pT = tr->GetDecimal(TString::Format("%s:pt", GetName().Data()), i),
              eta = tr->GetDecimal(TString::Format("%s:eta", GetName().Data()), i),
              phi = tr->GetDecimal(TString::Format("%s:phi", GetName().Data()), i);
  return makeTLVFromPtEtaPhiE(pT, eta, phi, e);
}

void IA0011::Exec (Option_t* /*option*/) {
  AnalysisTreeReader *tr = GetRawData();
  unsigned n = tr->GetDim(TString::Format("%s:pt", GetName().Data()));

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0011::MakeTLV (unsigned i) {
  AnalysisTreeReader *tr = GetRawData();

  long double e = tr->GetDecimal(TString::Format("%s:e", GetName().Data()), i),
              pT = tr->GetDecimal(TString::Format("%s:pt", GetName().Data()), i),
              eta = tr->GetDecimal(TString::Format("%s:eta", GetName().Data()), i),
              phi = tr->GetDecimal(TString::Format("%s:phi", GetName().Data()), i);
  return makeTLVFromPtEtaPhiE(pT, eta, phi, e);
}

void IA0012::Exec (Option_t* /*option*/) {
  unsigned n = fNEntries;

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0012::MakeTLV (unsigned i) {
  AnalysisTreeReader *tr = GetRawData();

  long double e = tr->GetDecimal(TString::Format("%s:e", GetName().Data()), i),
              pT = tr->GetDecimal(TString::Format("%s:pt", GetName().Data()), i),
              eta = tr->GetDecimal(TString::Format("%s:eta", GetName().Data()), i),
              phi = tr->GetDecimal(TString::Format("%s:phi", GetName().Data()), i);
  return makeTLVFromPtEtaPhiE(pT, eta, phi, e);
}

void IA0020::Exec (Option_t* /*option*/) {
  AnalysisTreeReader *tr = GetRawData();
  unsigned n = tr->GetInteger(TString::Format("%s:nentries", GetName().Data()));

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0020::MakeTLV (unsigned i) {
  AnalysisTreeReader *tr = GetRawData();

  long double m = tr->GetDecimal(TString::Format("%s:m", GetName().Data()), i),
              pT = tr->GetDecimal(TString::Format("%s:pT", GetName().Data()), i),
              eta = tr->GetDecimal(TString::Format("%s:eta", GetName().Data()), i),
              phi = tr->GetDecimal(TString::Format("%s:phi", GetName().Data()), i);
  return makeTLVFromPtEtaPhiM(pT, eta, phi, m);
}

void IA0021::Exec (Option_t* /*option*/) {
  AnalysisTreeReader *tr = GetRawData();
  unsigned n = tr->GetDim(TString::Format("%s:pt", GetName().Data()));

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0021::MakeTLV (unsigned i) {
  AnalysisTreeReader *tr = GetRawData();

  long double m = tr->GetDecimal(TString::Format("%s:m", GetName().Data()), i),
              pT = tr->GetDecimal(TString::Format("%s:pt", GetName().Data()), i),
              eta = tr->GetDecimal(TString::Format("%s:eta", GetName().Data()), i),
              phi = tr->GetDecimal(TString::Format("%s:phi", GetName().Data()), i);
  return makeTLVFromPtEtaPhiM(pT, eta, phi, m);
}

void IA0022::Exec (Option_t* /*option*/) {
  unsigned n = fNEntries;

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0022::MakeTLV (unsigned i) {
  AnalysisTreeReader *tr = GetRawData();

  long double m = tr->GetDecimal(TString::Format("%s:m", GetName().Data()), i),
              pT = tr->GetDecimal(TString::Format("%s:pt", GetName().Data()), i),
              eta = tr->GetDecimal(TString::Format("%s:eta", GetName().Data()), i),
              phi = tr->GetDecimal(TString::Format("%s:phi", GetName().Data()), i);
  return makeTLVFromPtEtaPhiM(pT, eta, phi, m);
}

/*
 * Filtering Algorithms
 * */
TString FA0000::SortTag () {
  return "pt";
}

bool FA0000::operator() (long long lhs, long long rhs) {
  AnalysisData *data = GetUserData();
  TLorentzVector *lhs_vec = (TLorentzVector*)data->GetTObject(fElementName.Data(), lhs);
  TLorentzVector *rhs_vec = (TLorentzVector*)data->GetTObject(fElementName.Data(), rhs);

  // decending order (greatest to least)
  return (lhs_vec->Pt() > rhs_vec->Pt());
}

void FA0000::Sort (std::vector<long long> &ip) {
  std::stable_sort(ip.begin(), ip.end(), *this);
}

/*
 * Reconstruction Algorithms
 * */
RA0000::RA0000 (TString name, TString title, long long length, ...) :
    Algorithm(name, title), fLength(length) {
  fParentNames = new const char*[fLength];
  va_list arguments;  // store the variable list of arguments

  va_start (arguments, length); // initializing arguments to store all values after length
  for (long long i = 0; i < fLength; ++i)
    fParentNames[i] = va_arg(arguments, const char*);
  va_end(arguments); // cleans up the list
}

RA0000::~RA0000() {
  delete[] fParentNames;
}

void RA0000::Exec (Option_t* /*option*/) {
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

void RA0000::Clear (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  TString VectorOutput = TString::Format("%s:4-vec", GetName().Data());
  TString NObjectsOutput = TString::Format("%s:nobjects", GetName().Data());

  if (data->Exists(VectorOutput.Data())) {
    for (long long i = 0; i < data->GetInteger(NObjectsOutput.Data()); ++i)
      delete data->GetTObject(VectorOutput.Data(), i);
  }
  data->RemoveAllAssociatedData(GetName().Data());
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

CA0100::CA0100 (TString name, TString title, long long length, ...) :
  CutAlgorithm(name, title), fLength(length) {
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
    else if (data->GetInteger(NObjects.Data()) < 1) {
      Abort();
      return;
    }
  }

  Passed();
}

CA0101::CA0101 (TString name, TString title, long long length, ...) :
  CutAlgorithm(name, title), fLength(length) {
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
    if (data->Exists(NObjects.Data()) && data->GetInteger(NObjects.Data()) > 0) {
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

double EA0003::StoreValue (TLorentzVector *vec) {
  return vec->M();
}

double EA0010::StoreValue (TLorentzVector *vec) {
  return vec->Pt();
}

double EA0013::StoreValue (TLorentzVector *vec) {
  return vec->M();
}

} /* HAL */ 
