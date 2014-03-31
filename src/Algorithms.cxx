#include <HAL/Algorithms.h>

namespace HAL
{

/*
 * Generic classes
 * */
void ImportTLVAlgo::Exec (unsigned n) {
  HAL::AnalysisData *data = (HAL::AnalysisData*)GetData("UserData");
  TString VectorOutput = TString::Format("%s:4-vec", GetName().Data());
  TString IndexOutput = TString::Format("%s:index", GetName().Data());
  TString NObjectsOutput = TString::Format("%s:nobjects", GetName().Data());

  for (unsigned i = 0; i < n; ++i) {
    TLorentzVector *vec = MakeTLV(i);
    data->SetValue(VectorOutput.Data(), // name of algorithm output
                   vec, // vector to store
                   i); // index of 4-vector
    data->SetValue(IndexOutput.Data(), // name of algorithm ouput
                   i, // store index separately
                   i); // index of index
  }
  data->SetValue(NObjectsOutput.Data(), // name of algorithm output
                 n); // total number of 4-vectors 
}

void ImportTLVAlgo::Clear (Option_t* /*option*/) {
  HAL::AnalysisData *data = (HAL::AnalysisData*)GetData("UserData");
  unsigned n = data->GetInteger(TString::Format("%s:nobjects", GetName().Data()).Data());
  TString VectorOutput = TString::Format("%s:4-vec", GetName().Data());

  for (unsigned i = 0; i < n; ++i) {
    delete data->GetTObject(VectorOutput.Data());
  }
}



/*
 * Actual classes
 * */
void IA0000::Exec (Option_t* /*option*/) {
  HAL::AnalysisTreeReader *tr = (HAL::AnalysisTreeReader*)GetData("RawData");
  unsigned n = tr->GetInteger(TString::Format("%s:nentries", GetName().Data()));

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0000::MakeTLV (unsigned i) {
  HAL::AnalysisTreeReader *tr = (HAL::AnalysisTreeReader*)GetData("RawData");

  long double x0 = tr->GetDecimal(TString::Format("%s:x0", GetName().Data()), i),
              x1 = tr->GetDecimal(TString::Format("%s:x1", GetName().Data()), i),
              x2 = tr->GetDecimal(TString::Format("%s:x2", GetName().Data()), i),
              x3 = tr->GetDecimal(TString::Format("%s:x3", GetName().Data()), i);
  return new TLorentzVector(x1, x2, x3, x0);
}

void IA0001::Exec (Option_t* /*option*/) {
  HAL::AnalysisTreeReader *tr = (HAL::AnalysisTreeReader*)GetData("RawData");
  unsigned n = tr->GetDim(TString::Format("%s:x1", GetName().Data()));

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0001::MakeTLV (unsigned i) {
  HAL::AnalysisTreeReader *tr = (HAL::AnalysisTreeReader*)GetData("RawData");

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
  HAL::AnalysisTreeReader *tr = (HAL::AnalysisTreeReader*)GetData("RawData");

  long double x0 = tr->GetDecimal(TString::Format("%s:x0", GetName().Data()), i),
              x1 = tr->GetDecimal(TString::Format("%s:x1", GetName().Data()), i),
              x2 = tr->GetDecimal(TString::Format("%s:x2", GetName().Data()), i),
              x3 = tr->GetDecimal(TString::Format("%s:x3", GetName().Data()), i);
  return new TLorentzVector(x1, x2, x3, x0);
}

void IA0010::Exec (Option_t* /*option*/) {
  HAL::AnalysisTreeReader *tr = (HAL::AnalysisTreeReader*)GetData("RawData");
  unsigned n = tr->GetInteger(TString::Format("%s:nentries", GetName().Data()));

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0010::MakeTLV (unsigned i) {
  HAL::AnalysisTreeReader *tr = (HAL::AnalysisTreeReader*)GetData("RawData");

  long double e = tr->GetDecimal(TString::Format("%s:e", GetName().Data()), i),
              pT = tr->GetDecimal(TString::Format("%s:pT", GetName().Data()), i),
              eta = tr->GetDecimal(TString::Format("%s:eta", GetName().Data()), i),
              phi = tr->GetDecimal(TString::Format("%s:phi", GetName().Data()), i);
  return makeTLVFromPtEtaPhiE(pT, eta, phi, e);
}

void IA0011::Exec (Option_t* /*option*/) {
  HAL::AnalysisTreeReader *tr = (HAL::AnalysisTreeReader*)GetData("RawData");
  unsigned n = tr->GetDim(TString::Format("%s:x1", GetName().Data()));

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0011::MakeTLV (unsigned i) {
  HAL::AnalysisTreeReader *tr = (HAL::AnalysisTreeReader*)GetData("RawData");

  long double e = tr->GetDecimal(TString::Format("%s:e", GetName().Data()), i),
              pT = tr->GetDecimal(TString::Format("%s:pT", GetName().Data()), i),
              eta = tr->GetDecimal(TString::Format("%s:eta", GetName().Data()), i),
              phi = tr->GetDecimal(TString::Format("%s:phi", GetName().Data()), i);
  return makeTLVFromPtEtaPhiE(pT, eta, phi, e);
}

void IA0012::Exec (Option_t* /*option*/) {
  unsigned n = fNEntries;

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0012::MakeTLV (unsigned i) {
  HAL::AnalysisTreeReader *tr = (HAL::AnalysisTreeReader*)GetData("RawData");

  long double e = tr->GetDecimal(TString::Format("%s:e", GetName().Data()), i),
              pT = tr->GetDecimal(TString::Format("%s:pT", GetName().Data()), i),
              eta = tr->GetDecimal(TString::Format("%s:eta", GetName().Data()), i),
              phi = tr->GetDecimal(TString::Format("%s:phi", GetName().Data()), i);
  return makeTLVFromPtEtaPhiE(pT, eta, phi, e);
}

void IA0020::Exec (Option_t* /*option*/) {
  HAL::AnalysisTreeReader *tr = (HAL::AnalysisTreeReader*)GetData("RawData");
  unsigned n = tr->GetInteger(TString::Format("%s:nentries", GetName().Data()));

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0020::MakeTLV (unsigned i) {
  HAL::AnalysisTreeReader *tr = (HAL::AnalysisTreeReader*)GetData("RawData");

  long double m = tr->GetDecimal(TString::Format("%s:m", GetName().Data()), i),
              pT = tr->GetDecimal(TString::Format("%s:pT", GetName().Data()), i),
              eta = tr->GetDecimal(TString::Format("%s:eta", GetName().Data()), i),
              phi = tr->GetDecimal(TString::Format("%s:phi", GetName().Data()), i);
  return makeTLVFromPtEtaPhiM(pT, eta, phi, m);
}

void IA0021::Exec (Option_t* /*option*/) {
  HAL::AnalysisTreeReader *tr = (HAL::AnalysisTreeReader*)GetData("RawData");
  unsigned n = tr->GetDim(TString::Format("%s:x1", GetName().Data()));

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0021::MakeTLV (unsigned i) {
  HAL::AnalysisTreeReader *tr = (HAL::AnalysisTreeReader*)GetData("RawData");

  long double m = tr->GetDecimal(TString::Format("%s:m", GetName().Data()), i),
              pT = tr->GetDecimal(TString::Format("%s:pT", GetName().Data()), i),
              eta = tr->GetDecimal(TString::Format("%s:eta", GetName().Data()), i),
              phi = tr->GetDecimal(TString::Format("%s:phi", GetName().Data()), i);
  return makeTLVFromPtEtaPhiM(pT, eta, phi, m);
}

void IA0022::Exec (Option_t* /*option*/) {
  unsigned n = fNEntries;

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0022::MakeTLV (unsigned i) {
  HAL::AnalysisTreeReader *tr = (HAL::AnalysisTreeReader*)GetData("RawData");

  long double m = tr->GetDecimal(TString::Format("%s:m", GetName().Data()), i),
              pT = tr->GetDecimal(TString::Format("%s:pT", GetName().Data()), i),
              eta = tr->GetDecimal(TString::Format("%s:eta", GetName().Data()), i),
              phi = tr->GetDecimal(TString::Format("%s:phi", GetName().Data()), i);
  return makeTLVFromPtEtaPhiM(pT, eta, phi, m);
}

void FA0000::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = (HAL::AnalysisData*)GetData("UserData");
  unsigned n = data->GetInteger(TString::Format("%s:nobjects", fInput.Data()).Data());
  TString VectorInput = TString::Format("%s:4-vec", fInput.Data());
  TString IndexInput = TString::Format("%s:index", fInput.Data());
  TString VectorOutput = TString::Format("%s:4-vec", GetName().Data());
  TString IndexOutput = TString::Format("%s:index", GetName().Data());
  double max_pt = 0.0;

  if (n < fN)
    throw HALException("n must be less than length of list");

  for (unsigned i = 0; i < n; ++i) {
    TLorentzVector *vec = (TLorentzVector*)data->GetTObject(VectorInput.Data(), i);
    double pt = vec->Pt();
    unsigned index = data->GetCounting(IndexInput.Data(), i);
    if (pt > max_pt) {
      max_pt = pt;
      data->SetValue(VectorOutput.Data(), vec);
      data->SetValue(IndexOutput.Data(), index);
    }
  }
}

//virtual void Exec (Option_t* option) {
//  HAL::AnalysisData *data = (HAL::AnalysisData*)GetData("UserData");
//  HAL::AnalysisTreeReader *tr = (HAL::AnalysisTreeReader*)GetData("RawData");
//  TLorentzVector *jet_1 = NULL, *jet_2 = NULL, *di_jet = NULL, *temp = NULL;
//  int jet_1_i, jet_2_i;
//
//  // Find leading and subleading jet in pT
//  for (int i = 0; i < tr->GetInteger("jet_n"); ++i) {
//    temp = HAL::makeTLVFromPtEtaPhiM(tr->GetDecimal("jet_pt", i), 
//        tr->GetDecimal("jet_eta", i), 
//        tr->GetDecimal("jet_phi", i), 
//        tr->GetDecimal("jet_m", i));
//    if (jet_1 == NULL) {jet_1_i = i; jet_1 = temp; continue;}
//    if (jet_1->Pt() < temp->Pt()) {
//      if (jet_2 != NULL) delete jet_2;
//      jet_2 = jet_1;
//      jet_2_i = jet_1_i;
//      jet_1 = temp;
//      jet_1_i = i;
//      continue;
//    }
//    if (jet_2 == NULL) {jet_2 = temp; continue;}
//    delete temp;
//  }
//
//  // Construct di-jet object
//  if (jet_1 != NULL && jet_2 != NULL) {
//    di_jet = jet_1;
//    di_jet->operator+=(*jet_2);
//    delete jet_2;
//  }
//
//  // Store data
//  data->SetValue("di-jet p", di_jet);
//  data->SetValue("di-jet leading index", jet_1_i);
//  data->SetValue("di-jet subleading index", jet_2_i);
//}
//
//virtual void Clear (Option_t* option) {
//  HAL::AnalysisData *data = (HAL::AnalysisData*)GetData("UserData");
//
//  delete data->GetTObject("di-jet p");
//}

} /* HAL */ 
