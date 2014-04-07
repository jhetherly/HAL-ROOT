#include <HAL/AnalysisTreeWriter.h>
#include <deque>

ClassImp(HAL::AnalysisTreeWriter);

namespace HAL
{

AnalysisTreeWriter::AnalysisTreeWriter (TString ofile) : 
  fCount(0), fOutputFileName(ofile) {}

void  AnalysisTreeWriter::SetValue (std::string n, bool v) {
  AnalysisData::SetValue(n, v, fCount);
}

void  AnalysisTreeWriter::SetValue (std::string n, bool v, long long i) {
  AnalysisData::SetValue(n, v, fCount, i);
}

//void  AnalysisTreeWriter::Set1DValue (std::string n, bool v) {
//  if (!Exists(n, fCount, 0))
//    fBoolIntCount[n][fCount] = 0;
//  AnalysisData::SetValue(n, v, fCount, fBoolIntCount[n][fCount]++);
//}

void  AnalysisTreeWriter::SetValue (std::string n, long double v) {
  AnalysisData::SetValue(n, v, fCount);
}

void  AnalysisTreeWriter::SetValue (std::string n, long double v, long long i) {
  AnalysisData::SetValue(n, v, fCount, i);
}

//void  AnalysisTreeWriter::Set1DValue (std::string n, long double v) {
//  if (!Exists(n, fCount, 0))
//    fDecimalIntCount[n][fCount] = 0;
//  AnalysisData::SetValue(n, v, fCount, fDecimalIntCount[n][fCount]++);
//}

void  AnalysisTreeWriter::SetValue (std::string n, long long v) {
  AnalysisData::SetValue(n, v, fCount);
}

void  AnalysisTreeWriter::SetValue (std::string n, long long v, long long i) {
  AnalysisData::SetValue(n, v, fCount, i);
}

//void  AnalysisTreeWriter::Set1DValue (std::string n, long long v) {
//  if (!Exists(n, fCount, 0))
//    fIntegerIntCount[n][fCount] = 0;
//  AnalysisData::SetValue(n, v, fCount, fIntegerIntCount[n][fCount]++);
//}

void  AnalysisTreeWriter::SetValue (std::string n, unsigned long long v) {
  AnalysisData::SetValue(n, v, fCount);
}

void  AnalysisTreeWriter::SetValue (std::string n, unsigned long long v, long long i) {
  AnalysisData::SetValue(n, v, fCount, i);
}

//void  AnalysisTreeWriter::SetValue (std::string n, unsigned long long v) {
//  if (!Exists(n, fCount, 0))
//    fCountingIntCount[n][fCount] = 0;
//  AnalysisData::SetValue(n, v, fCount, fCountingIntCount[n][fCount]++);
//}

void AnalysisTreeWriter::WriteData() {
  TFile f(fOutputFileName.Data(), "RECREATE");
  TTree t(fTreeName.Data(), fTreeDescription.Data());
  bool fill = false;

  std::map<std::string, bool> bBoolValues;
  std::map<std::string, std::vector<bool> > bBoolIntValues;
  std::map<std::string, double> bDecimalValues;
  std::map<std::string, std::vector<double> > bDecimalIntValues;
  std::map<std::string, long long> bIntegerValues;
  std::map<std::string, std::vector<long long> > bIntegerIntValues;
  std::map<std::string, unsigned long long> bCountingValues;
  std::map<std::string, std::vector<unsigned long long> > bCountingIntValues;
  
  // Loop over maps - each key is a branch name
  for (std::map<std::string, std::map<long long, bool> >::iterator outer = fBoolIntMap.begin();
      outer != fBoolIntMap.end(); ++outer) {
    std::string bname = outer->first;
    std::cout << "Creating branch " << bname << " of type bool" << std::endl;
    t.Branch(bname.c_str(), &bBoolValues[bname]);
  }
  for (std::map<std::string, std::map<long long, long double> >::iterator outer = fDecimalIntMap.begin();
      outer != fDecimalIntMap.end(); ++outer) {
    std::string bname = outer->first;
    std::cout << "Creating branch " << bname << " of type double" << std::endl;
    t.Branch(bname.c_str(), &bDecimalValues[bname]);
  }
  for (std::map<std::string, std::map<long long, long long> >::iterator outer = fIntegerIntMap.begin();
      outer != fIntegerIntMap.end(); ++outer) {
    std::string bname = outer->first;
    std::cout << "Creating branch " << bname << " of type long long" << std::endl;
    t.Branch(bname.c_str(), &bIntegerValues[bname]);
  }
  for (std::map<std::string, std::map<long long, unsigned long long> >::iterator outer = fCountingIntMap.begin();
      outer != fCountingIntMap.end(); ++outer) {
    std::string bname = outer->first;
    std::cout << "Creating branch " << bname << " of type unsigned long long" << std::endl;
    t.Branch(bname.c_str(), &bCountingValues[bname]);
  }
  for (std::map<std::string, std::map<long long, std::map<long long, bool> > >::iterator outer = fBoolIntIntMap.begin();
      outer != fBoolIntIntMap.end(); ++outer) {
    std::string bname = outer->first;
    std::cout << "Creating branch " << bname << " of type vector<bool>" << std::endl;
    t.Branch(bname.c_str(), &bBoolIntValues[bname]);
  }
  for (std::map<std::string, std::map<long long, std::map<long long, long double> > >::iterator outer = fDecimalIntIntMap.begin();
      outer != fDecimalIntIntMap.end(); ++outer) {
    std::string bname = outer->first;
    std::cout << "Creating branch " << bname << " of type vector<double>" << std::endl;
    t.Branch(bname.c_str(), &bDecimalIntValues[bname]);
  }
  for (std::map<std::string, std::map<long long, std::map<long long, long long> > >::iterator outer = fIntegerIntIntMap.begin();
      outer != fIntegerIntIntMap.end(); ++outer) {
    std::string bname = outer->first;
    std::cout << "Creating branch " << bname << " of type vector<long long>" << std::endl;
    t.Branch(bname.c_str(), &bIntegerIntValues[bname]);
  }
  for (std::map<std::string, std::map<long long, std::map<long long, unsigned long long> > >::iterator outer = fCountingIntIntMap.begin();
      outer != fCountingIntIntMap.end(); ++outer) {
    std::string bname = outer->first;
    std::cout << "Creating branch " << bname << " of type vector<unsigned long long>" << std::endl;
    t.Branch(bname.c_str(), &bCountingIntValues[bname]);
  }

  // actual loop to fill in the TTree
  for (long long i = 1; i <= fCount; ++i) {
    for (std::map<std::string, std::map<long long, bool> >::iterator outer = fBoolIntMap.begin();
        outer != fBoolIntMap.end(); ++outer) {
      std::string bname = outer->first;
      if (Exists(bname, i)) {
        fill = true;
        t.SetBranchStatus(bname.c_str(), 1);
        bBoolValues[bname] = GetBool(bname, i);
      }
      else
        t.SetBranchStatus(bname.c_str(), 0);
    }
    for (std::map<std::string, std::map<long long, long double> >::iterator outer = fDecimalIntMap.begin();
        outer != fDecimalIntMap.end(); ++outer) {
      std::string bname = outer->first;
      if (Exists(bname, i)) {
        fill = true;
        t.SetBranchStatus(bname.c_str(), 1);
        bDecimalValues[bname] = GetDecimal(bname, i);
      }
      else
        t.SetBranchStatus(bname.c_str(), 0);
    }
    for (std::map<std::string, std::map<long long, long long> >::iterator outer = fIntegerIntMap.begin();
        outer != fIntegerIntMap.end(); ++outer) {
      std::string bname = outer->first;
      if (Exists(bname, i)) {
        fill = true;
        t.SetBranchStatus(bname.c_str(), 1);
        bIntegerValues[bname] = GetInteger(bname, i);
      }
      else
        t.SetBranchStatus(bname.c_str(), 0);
    }
    for (std::map<std::string, std::map<long long, unsigned long long> >::iterator outer = fCountingIntMap.begin();
        outer != fCountingIntMap.end(); ++outer) {
      std::string bname = outer->first;
      if (Exists(bname, i)) {
        fill = true;
        t.SetBranchStatus(bname.c_str(), 1);
        bCountingValues[bname] = GetCounting(bname, i);
      }
      else
        t.SetBranchStatus(bname.c_str(), 0);
    }
    for (std::map<std::string, std::map<long long, std::map<long long, bool> > >::iterator outer = fBoolIntIntMap.begin();
        outer != fBoolIntIntMap.end(); ++outer) {
      std::string bname = outer->first;
      if (Exists(bname, i)) {
        fill = true;
        std::vector<bool> temp;
        for (std::map<long long, bool>::iterator it = outer->second[i].begin();
             it != outer->second[i].end(); ++it)
          temp.push_back(it->second);
        t.SetBranchStatus(bname.c_str(), 1);
        bBoolIntValues[bname] = temp;
      }
      else
        t.SetBranchStatus(bname.c_str(), 0);
    }
    for (std::map<std::string, std::map<long long, std::map<long long, long double> > >::iterator outer = fDecimalIntIntMap.begin();
        outer != fDecimalIntIntMap.end(); ++outer) {
      std::string bname = outer->first;
      if (Exists(bname, i)) {
        fill = true;
        std::vector<double> temp;
        for (std::map<long long, long double>::iterator it = outer->second[i].begin();
             it != outer->second[i].end(); ++it)
          temp.push_back(it->second);
        t.SetBranchStatus(bname.c_str(), 1);
        bDecimalIntValues[bname] = temp;
      }
      else
        t.SetBranchStatus(bname.c_str(), 0);
    }
    for (std::map<std::string, std::map<long long, std::map<long long, long long> > >::iterator outer = fIntegerIntIntMap.begin();
        outer != fIntegerIntIntMap.end(); ++outer) {
      std::string bname = outer->first;
      if (Exists(bname, i)) {
        fill = true;
        std::vector<long long> temp;
        for (std::map<long long, long long>::iterator it = outer->second[i].begin();
             it != outer->second[i].end(); ++it)
          temp.push_back(it->second);
        t.SetBranchStatus(bname.c_str(), 1);
        bIntegerIntValues[bname] = temp;
      }
      else
        t.SetBranchStatus(bname.c_str(), 0);
    }
    for (std::map<std::string, std::map<long long, std::map<long long, unsigned long long> > >::iterator outer = fCountingIntIntMap.begin();
        outer != fCountingIntIntMap.end(); ++outer) {
      std::string bname = outer->first;
      if (Exists(bname, i)) {
        fill = true;
        std::vector<unsigned long long> temp;
        for (std::map<long long, unsigned long long>::iterator it = outer->second[i].begin();
             it != outer->second[i].end(); ++it)
          temp.push_back(it->second);
        t.SetBranchStatus(bname.c_str(), 1);
        bCountingIntValues[bname] = temp;
      }
      else
        t.SetBranchStatus(bname.c_str(), 0);
    }

    if (fill)
      t.Fill();
    fill = false;
  }

  t.SetBranchStatus("*", kTRUE);
  f.Write();
  f.Close();
}

} /* HAL */ 

