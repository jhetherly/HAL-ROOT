#include <HAL/AnalysisTreeWriter.h>

ClassImp(HAL::AnalysisTreeWriter);

namespace HAL
{

AnalysisTreeWriter::AnalysisTreeWriter (TString ofile) : 
  fOutputFileName(ofile) {}

void  AnalysisTreeWriter::SetValue (std::string n, bool v) {
  if (fBoolCount.count(n) == 0)
    fBoolCount[n] = 0;
  AnalysisData::SetValue(n, v, ++fBoolCount[n]);
}

void  AnalysisTreeWriter::SetValue (std::string n, bool v, long long i) {
  if (fBoolIntCount.count(n) != 0 && fBoolIntCount[n].count(i) == 0)
    fBoolIntCount[n][i] = 0;
  else if (fBoolIntCount.count(n) == 0) {
    std::map<long long, long long> temp;
    temp.insert(std::pair<long long, long long>(i, 0));
    fBoolIntCount[n] = temp;
  }
  AnalysisData::SetValue(n, v, i, ++fBoolIntCount[n][i]);
}

void  AnalysisTreeWriter::SetValue (std::string n, long double v) {
  if (fDecimalCount.count(n) == 0)
    fDecimalCount[n] = 0;
  AnalysisData::SetValue(n, v, ++fDecimalCount[n]);
}

void  AnalysisTreeWriter::SetValue (std::string n, long double v, long long i) {
  if (fDecimalIntCount.count(n) != 0 && fDecimalIntCount[n].count(i) == 0)
    fDecimalIntCount[n][i] = 0;
  else if (fDecimalIntCount.count(n) == 0) {
    std::map<long long, long long> temp;
    temp.insert(std::pair<long long, long long>(i, 0));
    fDecimalIntCount[n] = temp;
  }
  AnalysisData::SetValue(n, v, i, ++fDecimalIntCount[n][i]);
}

void  AnalysisTreeWriter::SetValue (std::string n, long long v) {
  if (fIntegerCount.count(n) == 0)
    fIntegerCount[n] = 0;
  AnalysisData::SetValue(n, v, ++fIntegerCount[n]);
}

void  AnalysisTreeWriter::SetValue (std::string n, long long v, long long i) {
  if (fIntegerIntCount.count(n) != 0 && fIntegerIntCount[n].count(i) == 0)
    fIntegerIntCount[n][i] = 0;
  else if (fIntegerIntCount.count(n) == 0) {
    std::map<long long, long long> temp;
    temp.insert(std::pair<long long, long long>(i, 0));
    fIntegerIntCount[n] = temp;
  }
  AnalysisData::SetValue(n, v, i, ++fIntegerIntCount[n][i]);
}

void  AnalysisTreeWriter::SetValue (std::string n, unsigned long long v) {
  if (fCountingCount.count(n) == 0)
    fCountingCount[n] = 0;
  AnalysisData::SetValue(n, v, ++fCountingCount[n]);
}

void  AnalysisTreeWriter::SetValue (std::string n, unsigned long long v, long long i) {
  if (fCountingIntCount.count(n) != 0 && fCountingIntCount[n].count(i) == 0)
    fCountingIntCount[n][i] = 0;
  else if (fCountingIntCount.count(n) == 0) {
    std::map<long long, long long> temp;
    temp.insert(std::pair<long long, long long>(i, 0));
    fCountingIntCount[n] = temp;
  }
  AnalysisData::SetValue(n, v, i, ++fCountingIntCount[n][i]);
}

void AnalysisTreeWriter::WriteData() {
  TFile f(fOutputFileName.Data(), "RECREATE");
  TTree t(fTreeName.Data(), fTreeDescription.Data());

  // Loop over maps - each key is a branch name
  for (std::map<std::string, std::map<long long, bool> >::iterator outer = fBoolIntMap.begin();
      outer != fBoolIntMap.end(); ++outer) {
    std::string bname = outer->first;
    std::vector<bool> *values = new std::vector<bool>;

    for (std::map<long long, bool>::iterator inner = outer->second.begin();
        inner != outer->second.end(); ++inner) {
      bool value = inner->second;
      values->push_back(value);
    }
    t.Branch(bname.c_str(), "vector<bool>", &values);
  }
  for (std::map<std::string, std::map<long long, long double> >::iterator outer = fDecimalIntMap.begin();
      outer != fDecimalIntMap.end(); ++outer) {
    std::string bname = outer->first;
    //std::vector<long double> *values = new std::vector<long double>;
    std::vector<double> *values = new std::vector<double>;

    for (std::map<long long, long double>::iterator inner = outer->second.begin();
        inner != outer->second.end(); ++inner) {
      long double value = inner->second;
      //values->push_back(value);
      values->push_back((double)value);
    }
    //t.Branch(bname.c_str(), "vector<long double>", &values);
    t.Branch(bname.c_str(), "vector<double>", &values);
  }
  for (std::map<std::string, std::map<long long, long long> >::iterator outer = fIntegerIntMap.begin();
      outer != fIntegerIntMap.end(); ++outer) {
    std::string bname = outer->first;
    std::vector<long long> *values = new std::vector<long long>;

    for (std::map<long long, long long>::iterator inner = outer->second.begin();
        inner != outer->second.end(); ++inner) {
      long long value = inner->second;
      values->push_back(value);
    }
    t.Branch(bname.c_str(), "vector<long long>", &values);
  }
  for (std::map<std::string, std::map<long long, unsigned long long> >::iterator outer = fCountingIntMap.begin();
      outer != fCountingIntMap.end(); ++outer) {
    std::string bname = outer->first;
    std::vector<unsigned long long> *values = new std::vector<unsigned long long>;

    for (std::map<long long, unsigned long long>::iterator inner = outer->second.begin();
        inner != outer->second.end(); ++inner) {
      unsigned long long value = inner->second;
      values->push_back(value);
    }
    t.Branch(bname.c_str(), "vector<unsigned long long>", &values);
  }
  for (std::map<std::string, std::map<long long, std::map<long long, bool> > >::iterator outer = fBoolIntIntMap.begin();
      outer != fBoolIntIntMap.end(); ++outer) {
    std::string bname = outer->first;
    std::vector<std::vector<bool> > *values = new std::vector<std::vector<bool> >;

    for (std::map<long long, std::map<long long, bool> >::iterator middle = outer->second.begin();
        middle != outer->second.end(); ++middle) {
      std::vector<bool> row;
      for (std::map<long long, bool>::iterator inner = middle->second.begin();
          inner != middle->second.end(); ++inner) {
        bool value = inner->second;
        row.push_back(value);
      }
      values->push_back(row);
    }
    t.Branch(bname.c_str(), "vector<vector<bool> >", &values);
  }
  for (std::map<std::string, std::map<long long, std::map<long long, long double> > >::iterator outer = fDecimalIntIntMap.begin();
      outer != fDecimalIntIntMap.end(); ++outer) {
    std::string bname = outer->first;
    //std::vector<std::vector<long double> > *values = new std::vector<std::vector<long double> >;
    std::vector<std::vector<double> > *values = new std::vector<std::vector<double> >;

    for (std::map<long long, std::map<long long, long double> >::iterator middle = outer->second.begin();
        middle != outer->second.end(); ++middle) {
      //std::vector<long double> row;
      std::vector<double> row;
      for (std::map<long long, long double>::iterator inner = middle->second.begin();
          inner != middle->second.end(); ++inner) {
        long double value = inner->second;
        //row.push_back(value);
        row.push_back((double)value);
      }
      values->push_back(row);
    }
    //t.Branch(bname.c_str(), "vector<vector<long double> >", &values);
    t.Branch(bname.c_str(), "vector<vector<double> >", &values);
  }
  for (std::map<std::string, std::map<long long, std::map<long long, long long> > >::iterator outer = fIntegerIntIntMap.begin();
      outer != fIntegerIntIntMap.end(); ++outer) {
    std::string bname = outer->first;
    std::vector<std::vector<long long> > *values = new std::vector<std::vector<long long> >;

    for (std::map<long long, std::map<long long, long long> >::iterator middle = outer->second.begin();
        middle != outer->second.end(); ++middle) {
      std::vector<long long> row;
      for (std::map<long long, long long>::iterator inner = middle->second.begin();
          inner != middle->second.end(); ++inner) {
        long long value = inner->second;
        row.push_back(value);
      }
      values->push_back(row);
    }
    t.Branch(bname.c_str(), "vector<vector<long long> >", &values);
  }
  for (std::map<std::string, std::map<long long, std::map<long long, unsigned long long> > >::iterator outer = fCountingIntIntMap.begin();
      outer != fCountingIntIntMap.end(); ++outer) {
    std::string bname = outer->first;
    std::vector<std::vector<unsigned long long> > *values = new std::vector<std::vector<unsigned long long> >;

    for (std::map<long long, std::map<long long, unsigned long long> >::iterator middle = outer->second.begin();
        middle != outer->second.end(); ++middle) {
      std::vector<unsigned long long> row;
      for (std::map<long long, unsigned long long>::iterator inner = middle->second.begin();
          inner != middle->second.end(); ++inner) {
        unsigned long long value = inner->second;
        row.push_back(value);
      }
      values->push_back(row);
    }
    t.Branch(bname.c_str(), "vector<vector<unsigned long long> >", &values);
  }

  t.Fill();
  t.Write();
  f.Close();
}

} /* HAL */ 

