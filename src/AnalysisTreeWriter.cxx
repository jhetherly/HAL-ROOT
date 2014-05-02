#include <HAL/AnalysisTreeWriter.h>

ClassImp(HAL::AnalysisTreeWriter);

namespace HAL
{

AnalysisTreeWriter::AnalysisTreeWriter (const TString &ofile) : 
  fCount(0), fOutputFileName(ofile) {}

void  AnalysisTreeWriter::SetValue (const TString &n, const bool &v) {
  fTreeIndicesMap[fBranchTreeMap[n].EqualTo("") ? fTreeName : fBranchTreeMap[n]].insert(fCount);
  AnalysisData::SetValue(n, v, fCount);
}

void  AnalysisTreeWriter::SetValue (const TString &n, const bool &v, const long long &i) {
  fTreeIndicesMap[fBranchTreeMap[n].EqualTo("") ? fTreeName : fBranchTreeMap[n]].insert(fCount);
  AnalysisData::SetValue(n, v, fCount, i);
}

//void  AnalysisTreeWriter::Set1DValue (const TString &n, bool v) {
//  if (!Exists(n, fCount, 0))
//    fBoolIntCount[n][fCount] = 0;
//  AnalysisData::SetValue(n, v, fCount, fBoolIntCount[n][fCount]++);
//}

void  AnalysisTreeWriter::SetValue (const TString &n, const long double &v) {
  fTreeIndicesMap[fBranchTreeMap[n].EqualTo("") ? fTreeName : fBranchTreeMap[n]].insert(fCount);
  AnalysisData::SetValue(n, v, fCount);
}

void  AnalysisTreeWriter::SetValue (const TString &n, const long double &v, const long long &i) {
  fTreeIndicesMap[fBranchTreeMap[n].EqualTo("") ? fTreeName : fBranchTreeMap[n]].insert(fCount);
  AnalysisData::SetValue(n, v, fCount, i);
}

//void  AnalysisTreeWriter::Set1DValue (const TString &n, long double v) {
//  if (!Exists(n, fCount, 0))
//    fDecimalIntCount[n][fCount] = 0;
//  AnalysisData::SetValue(n, v, fCount, fDecimalIntCount[n][fCount]++);
//}

void  AnalysisTreeWriter::SetValue (const TString &n, const long long &v) {
  fTreeIndicesMap[fBranchTreeMap[n].EqualTo("") ? fTreeName : fBranchTreeMap[n]].insert(fCount);
  AnalysisData::SetValue(n, v, fCount);
}

void  AnalysisTreeWriter::SetValue (const TString &n, const long long &v, const long long &i) {
  fTreeIndicesMap[fBranchTreeMap[n].EqualTo("") ? fTreeName : fBranchTreeMap[n]].insert(fCount);
  AnalysisData::SetValue(n, v, fCount, i);
}

//void  AnalysisTreeWriter::Set1DValue (const TString &n, long long v) {
//  if (!Exists(n, fCount, 0))
//    fIntegerIntCount[n][fCount] = 0;
//  AnalysisData::SetValue(n, v, fCount, fIntegerIntCount[n][fCount]++);
//}

void  AnalysisTreeWriter::SetValue (const TString &n, const unsigned long long &v) {
  fTreeIndicesMap[fBranchTreeMap[n].EqualTo("") ? fTreeName : fBranchTreeMap[n]].insert(fCount);
  AnalysisData::SetValue(n, v, fCount);
}

void  AnalysisTreeWriter::SetValue (const TString &n, const unsigned long long &v, const long long &i) {
  fTreeIndicesMap[fBranchTreeMap[n].EqualTo("") ? fTreeName : fBranchTreeMap[n]].insert(fCount);
  AnalysisData::SetValue(n, v, fCount, i);
}

//void  AnalysisTreeWriter::SetValue (const TString &n, unsigned long long v) {
//  if (!Exists(n, fCount, 0))
//    fCountingIntCount[n][fCount] = 0;
//  AnalysisData::SetValue(n, v, fCount, fCountingIntCount[n][fCount]++);
//}

void AnalysisTreeWriter::WriteData() {
  TFile f(fOutputFileName.Data(), "RECREATE");
  std::map<TString, TTree*> trees;
  std::map<TString, bool> fill;

  std::map<std::string, bool> bBoolValues;
  std::map<std::string, std::vector<bool> > bBoolIntValues;
  std::map<std::string, double> bDecimalValues;
  std::map<std::string, std::vector<double> > bDecimalIntValues;
  std::map<std::string, long long> bIntegerValues;
  std::map<std::string, std::vector<long long> > bIntegerIntValues;
  std::map<std::string, unsigned long long> bCountingValues;
  std::map<std::string, std::vector<unsigned long long> > bCountingIntValues;
  
  // Loop over maps to record how many trees to create
  for (std::map<std::string, std::map<long long, bool>, internal::string_cmp>::iterator outer = fBoolIntMap.begin();
      outer != fBoolIntMap.end(); ++outer) {
    TString treeName = fBranchTreeMap[outer->first.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[outer->first.c_str()];
    if (trees.count(treeName) == 0) {
      trees[treeName] = new TTree(treeName, fTreeDescription.Data());
      fill[treeName] = false;
    }
  }
  for (std::map<std::string, std::map<long long, long double>, internal::string_cmp >::iterator outer = fDecimalIntMap.begin();
      outer != fDecimalIntMap.end(); ++outer) {
    TString treeName = fBranchTreeMap[outer->first.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[outer->first.c_str()];
    if (trees.count(treeName) == 0) {
      trees[treeName] = new TTree(treeName, fTreeDescription.Data());
      fill[treeName] = false;
    }
  }
  for (std::map<std::string, std::map<long long, long long>, internal::string_cmp >::iterator outer = fIntegerIntMap.begin();
      outer != fIntegerIntMap.end(); ++outer) {
    TString treeName = fBranchTreeMap[outer->first.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[outer->first.c_str()];
    if (trees.count(treeName) == 0) {
      trees[treeName] = new TTree(treeName, fTreeDescription.Data());
      fill[treeName] = false;
    }
  }
  for (std::map<std::string, std::map<long long, unsigned long long>, internal::string_cmp >::iterator outer = fCountingIntMap.begin();
      outer != fCountingIntMap.end(); ++outer) {
    TString treeName = fBranchTreeMap[outer->first.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[outer->first.c_str()];
    if (trees.count(treeName) == 0) {
      trees[treeName] = new TTree(treeName, fTreeDescription.Data());
      fill[treeName] = false;
    }
    //unsigned length = outer->second.size();
    //if (length > max_scalar_nentries) max_scalar_nentries = length;
    //TString treeName = fBranchTreeMap[outer->first].EqualTo("") ? fTreeName : fBranchTreeMap[outer->first];//fTreeName;
    //if (trees.count(length) == 0) {
    //  if (!trees.empty())
    //    treeName.Append(TString::Format("%u", ++treeCount));
    //  trees[length] = new TTree(treeName, fTreeDescription.Data());
    //  fill[length] = false;
    //}
  }
  for (std::map<std::string, std::map<long long, std::map<long long, bool> >, internal::string_cmp >::iterator outer = fBoolIntIntMap.begin();
      outer != fBoolIntIntMap.end(); ++outer) {
    TString treeName = fBranchTreeMap[outer->first.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[outer->first.c_str()];
    if (trees.count(treeName) == 0) {
      trees[treeName] = new TTree(treeName, fTreeDescription.Data());
      fill[treeName] = false;
    }
  }
  for (std::map<std::string, std::map<long long, std::map<long long, long double> >, internal::string_cmp >::iterator outer = fDecimalIntIntMap.begin();
      outer != fDecimalIntIntMap.end(); ++outer) {
    TString treeName = fBranchTreeMap[outer->first.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[outer->first.c_str()];
    if (trees.count(treeName) == 0) {
      trees[treeName] = new TTree(treeName, fTreeDescription.Data());
      fill[treeName] = false;
    }
  }
  for (std::map<std::string, std::map<long long, std::map<long long, long long> >, internal::string_cmp >::iterator outer = fIntegerIntIntMap.begin();
      outer != fIntegerIntIntMap.end(); ++outer) {
    TString treeName = fBranchTreeMap[outer->first.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[outer->first.c_str()];
    if (trees.count(treeName) == 0) {
      trees[treeName] = new TTree(treeName, fTreeDescription.Data());
      fill[treeName] = false;
    }
  }
  for (std::map<std::string, std::map<long long, std::map<long long, unsigned long long> >, internal::string_cmp >::iterator outer = fCountingIntIntMap.begin();
      outer != fCountingIntIntMap.end(); ++outer) {
    TString treeName = fBranchTreeMap[outer->first.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[outer->first.c_str()];
    if (trees.count(treeName) == 0) {
      trees[treeName] = new TTree(treeName, fTreeDescription.Data());
      fill[treeName] = false;
    }
  }

  // Loop over maps - each key is a branch name
  std::cout << std::endl;
  for (std::map<std::string, std::map<long long, bool>, internal::string_cmp >::iterator outer = fBoolIntMap.begin();
      outer != fBoolIntMap.end(); ++outer) {
    std::string bname = outer->first;
    TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
    std::cout << "Creating branch " << bname << " of type bool" << std::endl;
    std::cout << "Located in tree " << trees[tname]->GetName() << std::endl << std::endl;
    trees[tname]->Branch(bname.c_str(), &bBoolValues[bname]);
  }
  for (std::map<std::string, std::map<long long, long double>, internal::string_cmp >::iterator outer = fDecimalIntMap.begin();
      outer != fDecimalIntMap.end(); ++outer) {
    std::string bname = outer->first;
    TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
    std::cout << "Creating branch " << bname << " of type double" << std::endl;
    std::cout << "Located in tree " << trees[tname]->GetName() << std::endl << std::endl;
    trees[tname]->Branch(bname.c_str(), &bDecimalValues[bname]);
  }
  for (std::map<std::string, std::map<long long, long long>, internal::string_cmp >::iterator outer = fIntegerIntMap.begin();
      outer != fIntegerIntMap.end(); ++outer) {
    std::string bname = outer->first;
    TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
    std::cout << "Creating branch " << bname << " of type long long" << std::endl;
    std::cout << "Located in tree " << trees[tname]->GetName() << std::endl << std::endl;
    trees[tname]->Branch(bname.c_str(), &bIntegerValues[bname]);
  }
  for (std::map<std::string, std::map<long long, unsigned long long>, internal::string_cmp >::iterator outer = fCountingIntMap.begin();
      outer != fCountingIntMap.end(); ++outer) {
    std::string bname = outer->first;
    TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
    std::cout << "Creating branch " << bname << " of type unsigned long long" << std::endl;
    std::cout << "Located in tree " << trees[tname]->GetName() << std::endl << std::endl;
    trees[tname]->Branch(bname.c_str(), &bCountingValues[bname]);
  }
  for (std::map<std::string, std::map<long long, std::map<long long, bool> >, internal::string_cmp >::iterator outer = fBoolIntIntMap.begin();
      outer != fBoolIntIntMap.end(); ++outer) {
    std::string bname = outer->first;
    TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
    //unsigned length = outer->second.size();
    std::cout << "Creating branch " << bname << " of type vector<bool>" << std::endl;
    //std::cout << "Located in tree " << trees[max_scalar_nentries]->GetName() << std::endl << std::endl;
    std::cout << "Located in tree " << trees[tname]->GetName() << std::endl << std::endl;
    //std::cout << "Located in tree " << trees[length]->GetName() << std::endl << std::endl;
    trees[tname]->Branch(bname.c_str(), &bBoolIntValues[bname]);
    //trees[max_scalar_nentries]->Branch(bname.c_str(), &bBoolIntValues[bname]);
    //trees[length]->Branch(bname.c_str(), &bBoolIntValues[bname]);
  }
  for (std::map<std::string, std::map<long long, std::map<long long, long double> >, internal::string_cmp >::iterator outer = fDecimalIntIntMap.begin();
      outer != fDecimalIntIntMap.end(); ++outer) {
    std::string bname = outer->first;
    TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
    std::cout << "Creating branch " << bname << " of type vector<double>" << std::endl;
    std::cout << "Located in tree " << trees[tname]->GetName() << std::endl << std::endl;
    trees[tname]->Branch(bname.c_str(), &bDecimalIntValues[bname]);
  }
  for (std::map<std::string, std::map<long long, std::map<long long, long long> >, internal::string_cmp >::iterator outer = fIntegerIntIntMap.begin();
      outer != fIntegerIntIntMap.end(); ++outer) {
    std::string bname = outer->first;
    TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
    std::cout << "Creating branch " << bname << " of type vector<long long>" << std::endl;
    std::cout << "Located in tree " << trees[tname]->GetName() << std::endl << std::endl;
    trees[tname]->Branch(bname.c_str(), &bIntegerIntValues[bname]);
  }
  for (std::map<std::string, std::map<long long, std::map<long long, unsigned long long> >, internal::string_cmp >::iterator outer = fCountingIntIntMap.begin();
      outer != fCountingIntIntMap.end(); ++outer) {
    std::string bname = outer->first;
    TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
    std::cout << "Creating branch " << bname << " of type vector<unsigned long long>" << std::endl;
    std::cout << "Located in tree " << trees[tname]->GetName() << std::endl << std::endl;
    trees[tname]->Branch(bname.c_str(), &bCountingIntValues[bname]);
  }

  // actual loop to fill in the TTree
  for (long long i = 1; i <= fCount; ++i) {
    for (std::map<std::string, std::map<long long, bool>, internal::string_cmp >::iterator outer = fBoolIntMap.begin();
        outer != fBoolIntMap.end(); ++outer) {
      std::string bname = outer->first;
      TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
      if (fTreeIndicesMap[tname].count(i) != 0) {
        fill[tname] = true;
        trees[tname]->SetBranchStatus(bname.c_str(), 1);
        if (Exists(bname, i))
          bBoolValues[bname] = GetBool(bname, i);
        else // DEFAULT
          bBoolValues[bname] = false;
      }
      else
        trees[tname]->SetBranchStatus(bname.c_str(), 0);
    }
    for (std::map<std::string, std::map<long long, long double>, internal::string_cmp >::iterator outer = fDecimalIntMap.begin();
        outer != fDecimalIntMap.end(); ++outer) {
      std::string bname = outer->first;
      TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
      if (fTreeIndicesMap[tname].count(i) != 0) {
        fill[tname] = true;
        trees[tname]->SetBranchStatus(bname.c_str(), 1);
        if (Exists(bname, i))
          bDecimalValues[bname] = GetDecimal(bname, i);
        else // DEFAULT
          bDecimalValues[bname] = -536870912.0;
      }
      else
        trees[tname]->SetBranchStatus(bname.c_str(), 0);
    }
    for (std::map<std::string, std::map<long long, long long>, internal::string_cmp >::iterator outer = fIntegerIntMap.begin();
        outer != fIntegerIntMap.end(); ++outer) {
      std::string bname = outer->first;
      TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
      if (fTreeIndicesMap[tname].count(i) != 0) {
        fill[tname] = true;
        trees[tname]->SetBranchStatus(bname.c_str(), 1);
        if (Exists(bname, i))
          bIntegerValues[bname] = GetInteger(bname, i);
        else // DEFAULT
          bIntegerValues[bname] = -536870912;
      }
      else
        trees[tname]->SetBranchStatus(bname.c_str(), 0);
    }
    for (std::map<std::string, std::map<long long, unsigned long long>, internal::string_cmp >::iterator outer = fCountingIntMap.begin();
        outer != fCountingIntMap.end(); ++outer) {
      std::string bname = outer->first;
      TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
      if (fTreeIndicesMap[tname].count(i) != 0) {
        fill[tname] = true;
        trees[tname]->SetBranchStatus(bname.c_str(), 1);
        if (Exists(bname, i))
          bCountingValues[bname] = GetCounting(bname, i);
        else // DEFAULT
          bCountingValues[bname] = 536870912;
      }
      else
        trees[tname]->SetBranchStatus(bname.c_str(), 0);
    }
    for (std::map<std::string, std::map<long long, std::map<long long, bool> >, internal::string_cmp >::iterator outer = fBoolIntIntMap.begin();
        outer != fBoolIntIntMap.end(); ++outer) {
      std::string bname = outer->first;
      TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
      if (fTreeIndicesMap[tname].count(i) != 0) {
        std::vector<bool> temp;
        fill[tname] = true;
        trees[tname]->SetBranchStatus(bname.c_str(), 1);
        if (Exists(bname, i)) {
          for (std::map<long long, bool>::iterator it = outer->second[i].begin();
               it != outer->second[i].end(); ++it)
            temp.push_back(it->second);
        }
        bBoolIntValues[bname] = temp;
      }
      else
        trees[tname]->SetBranchStatus(bname.c_str(), 0);
    }
    for (std::map<std::string, std::map<long long, std::map<long long, long double> >, internal::string_cmp >::iterator outer = fDecimalIntIntMap.begin();
        outer != fDecimalIntIntMap.end(); ++outer) {
      std::string bname = outer->first;
      TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
      if (fTreeIndicesMap[tname].count(i) != 0) {
        std::vector<double> temp;
        fill[tname] = true;
        trees[tname]->SetBranchStatus(bname.c_str(), 1);
        if (Exists(bname, i)) {
          for (std::map<long long, long double>::iterator it = outer->second[i].begin();
               it != outer->second[i].end(); ++it)
            temp.push_back(it->second);
        }
        bDecimalIntValues[bname] = temp; 
      }
      else
        trees[tname]->SetBranchStatus(bname.c_str(), 0);
    }
    for (std::map<std::string, std::map<long long, std::map<long long, long long> >, internal::string_cmp >::iterator outer = fIntegerIntIntMap.begin();
        outer != fIntegerIntIntMap.end(); ++outer) {
      std::string bname = outer->first;
      TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
      if (fTreeIndicesMap[tname].count(i) != 0) {
        std::vector<long long> temp;
        fill[tname] = true;
        trees[tname]->SetBranchStatus(bname.c_str(), 1);
        if (Exists(bname, i)) {
          for (std::map<long long, long long>::iterator it = outer->second[i].begin();
               it != outer->second[i].end(); ++it)
            temp.push_back(it->second);
        }
        bIntegerIntValues[bname] = temp;
      }
      else
        trees[tname]->SetBranchStatus(bname.c_str(), 0);
    }
    for (std::map<std::string, std::map<long long, std::map<long long, unsigned long long> >, internal::string_cmp >::iterator outer = fCountingIntIntMap.begin();
        outer != fCountingIntIntMap.end(); ++outer) {
      std::string bname = outer->first;
      TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
      if (fTreeIndicesMap[tname].count(i) != 0) {
        std::vector<unsigned long long> temp;
        fill[tname] = true;
        trees[tname]->SetBranchStatus(bname.c_str(), 1);
        if (Exists(bname, i)) {
          for (std::map<long long, unsigned long long>::iterator it = outer->second[i].begin();
               it != outer->second[i].end(); ++it)
            temp.push_back(it->second);
        }
        bCountingIntValues[bname] = temp;
      }
      else
        trees[tname]->SetBranchStatus(bname.c_str(), 0);
    }

    // fill trees and reset fill flags
    for (std::map<TString, bool>::iterator it = fill.begin(); it != fill.end(); ++it) {
      if (it->second)
        trees[it->first]->Fill();
      it->second = false;
    }
  }

  for (std::map<TString, TTree*>::iterator it = trees.begin(); it != trees.end(); ++it)
    it->second->SetBranchStatus("*", kTRUE);
  f.Write();
  f.Close();
}

} /* HAL */ 

