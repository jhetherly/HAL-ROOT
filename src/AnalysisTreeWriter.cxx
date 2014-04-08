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
  //std::map<unsigned, TTree> trees;
  std::map<unsigned, TTree*> trees;
  std::map<unsigned, bool> fill;
  unsigned treeCount = 1;

  //TTree t(fTreeName.Data(), fTreeDescription.Data());
  //bool fill = false;

  std::map<std::string, bool> bBoolValues;
  std::map<std::string, std::vector<bool> > bBoolIntValues;
  std::map<std::string, double> bDecimalValues;
  std::map<std::string, std::vector<double> > bDecimalIntValues;
  std::map<std::string, long long> bIntegerValues;
  std::map<std::string, std::vector<long long> > bIntegerIntValues;
  std::map<std::string, unsigned long long> bCountingValues;
  std::map<std::string, std::vector<unsigned long long> > bCountingIntValues;
  
  // Loop over maps to record how many trees to create
  for (std::map<std::string, std::map<long long, bool> >::iterator outer = fBoolIntMap.begin();
      outer != fBoolIntMap.end(); ++outer) {
    unsigned length = outer->second.size();
    if (trees.count(length) == 0) {
      trees[length] = new TTree(TString::Format("%s%u", fTreeName.Data(), treeCount++).Data(),
          fTreeDescription.Data());
      //trees[length].SetName(TString::Format("%s%u", fTreeName.Data(), treeCount++).Data());
      //trees[length].SetTitle(fTreeDescription.Data());
      fill[length] = false;
    }
  }
  for (std::map<std::string, std::map<long long, long double> >::iterator outer = fDecimalIntMap.begin();
      outer != fDecimalIntMap.end(); ++outer) {
    unsigned length = outer->second.size();
    if (trees.count(length) == 0) {
      trees[length] = new TTree(TString::Format("%s%u", fTreeName.Data(), treeCount++).Data(),
          fTreeDescription.Data());
      //trees[length].SetName(TString::Format("%s%u", fTreeName.Data(), treeCount++).Data());
      //trees[length].SetTitle(fTreeDescription.Data());
      fill[length] = false;
    }
  }
  for (std::map<std::string, std::map<long long, long long> >::iterator outer = fIntegerIntMap.begin();
      outer != fIntegerIntMap.end(); ++outer) {
    unsigned length = outer->second.size();
    if (trees.count(length) == 0) {
      trees[length] = new TTree(TString::Format("%s%u", fTreeName.Data(), treeCount++).Data(),
          fTreeDescription.Data());
      //trees[length].SetName(TString::Format("%s%u", fTreeName.Data(), treeCount++).Data());
      //trees[length].SetTitle(fTreeDescription.Data());
      fill[length] = false;
    }
  }
  for (std::map<std::string, std::map<long long, unsigned long long> >::iterator outer = fCountingIntMap.begin();
      outer != fCountingIntMap.end(); ++outer) {
    unsigned length = outer->second.size();
    if (trees.count(length) == 0) {
      trees[length] = new TTree(TString::Format("%s%u", fTreeName.Data(), treeCount++).Data(),
          fTreeDescription.Data());
      //trees[length].SetName(TString::Format("%s%u", fTreeName.Data(), treeCount++).Data());
      //trees[length].SetTitle(fTreeDescription.Data());
      fill[length] = false;
    }
  }
  for (std::map<std::string, std::map<long long, std::map<long long, bool> > >::iterator outer = fBoolIntIntMap.begin();
      outer != fBoolIntIntMap.end(); ++outer) {
    unsigned length = outer->second.size();
    if (trees.count(length) == 0) {
      trees[length] = new TTree(TString::Format("%s%u", fTreeName.Data(), treeCount++).Data(),
          fTreeDescription.Data());
      //trees[length].SetName(TString::Format("%s%u", fTreeName.Data(), treeCount++).Data());
      //trees[length].SetTitle(fTreeDescription.Data());
      fill[length] = false;
    }
  }
  for (std::map<std::string, std::map<long long, std::map<long long, long double> > >::iterator outer = fDecimalIntIntMap.begin();
      outer != fDecimalIntIntMap.end(); ++outer) {
    unsigned length = outer->second.size();
    if (trees.count(length) == 0) {
      trees[length] = new TTree(TString::Format("%s%u", fTreeName.Data(), treeCount++).Data(),
          fTreeDescription.Data());
      //trees[length].SetName(TString::Format("%s%u", fTreeName.Data(), treeCount++).Data());
      //trees[length].SetTitle(fTreeDescription.Data());
      fill[length] = false;
    }
  }
  for (std::map<std::string, std::map<long long, std::map<long long, long long> > >::iterator outer = fIntegerIntIntMap.begin();
      outer != fIntegerIntIntMap.end(); ++outer) {
    unsigned length = outer->second.size();
    if (trees.count(length) == 0) {
      trees[length] = new TTree(TString::Format("%s%u", fTreeName.Data(), treeCount++).Data(),
          fTreeDescription.Data());
      //trees[length].SetName(TString::Format("%s%u", fTreeName.Data(), treeCount++).Data());
      //trees[length].SetTitle(fTreeDescription.Data());
      fill[length] = false;
    }
  }
  for (std::map<std::string, std::map<long long, std::map<long long, unsigned long long> > >::iterator outer = fCountingIntIntMap.begin();
      outer != fCountingIntIntMap.end(); ++outer) {
    unsigned length = outer->second.size();
    if (trees.count(length) == 0) {
      trees[length] = new TTree(TString::Format("%s%u", fTreeName.Data(), treeCount++).Data(),
          fTreeDescription.Data());
      //trees[length].SetName(TString::Format("%s%u", fTreeName.Data(), treeCount++).Data());
      //trees[length].SetTitle(fTreeDescription.Data());
      fill[length] = false;
    }
  }

  // Loop over maps - each key is a branch name
  std::cout << std::endl;
  for (std::map<std::string, std::map<long long, bool> >::iterator outer = fBoolIntMap.begin();
      outer != fBoolIntMap.end(); ++outer) {
    std::string bname = outer->first;
    unsigned length = outer->second.size();
    std::cout << "Creating branch " << bname << " of type bool" << std::endl;
    //std::cout << "Located in tree " << trees[length].GetName() << std::endl << std::endl;
    //trees[length].Branch(bname.c_str(), &bBoolValues[bname]);
    std::cout << "Located in tree " << trees[length]->GetName() << std::endl << std::endl;
    trees[length]->Branch(bname.c_str(), &bBoolValues[bname]);
  }
  for (std::map<std::string, std::map<long long, long double> >::iterator outer = fDecimalIntMap.begin();
      outer != fDecimalIntMap.end(); ++outer) {
    std::string bname = outer->first;
    unsigned length = outer->second.size();
    std::cout << "Creating branch " << bname << " of type double" << std::endl;
    //std::cout << "Located in tree " << trees[length].GetName() << std::endl << std::endl;
    //trees[length].Branch(bname.c_str(), &bDecimalValues[bname]);
    std::cout << "Located in tree " << trees[length]->GetName() << std::endl << std::endl;
    trees[length]->Branch(bname.c_str(), &bDecimalValues[bname]);
  }
  for (std::map<std::string, std::map<long long, long long> >::iterator outer = fIntegerIntMap.begin();
      outer != fIntegerIntMap.end(); ++outer) {
    std::string bname = outer->first;
    unsigned length = outer->second.size();
    std::cout << "Creating branch " << bname << " of type long long" << std::endl;
    //std::cout << "Located in tree " << trees[length].GetName() << std::endl << std::endl;
    //trees[length].Branch(bname.c_str(), &bIntegerValues[bname]);
    std::cout << "Located in tree " << trees[length]->GetName() << std::endl << std::endl;
    trees[length]->Branch(bname.c_str(), &bIntegerValues[bname]);
  }
  for (std::map<std::string, std::map<long long, unsigned long long> >::iterator outer = fCountingIntMap.begin();
      outer != fCountingIntMap.end(); ++outer) {
    std::string bname = outer->first;
    unsigned length = outer->second.size();
    std::cout << "Creating branch " << bname << " of type unsigned long long" << std::endl;
    //std::cout << "Located in tree " << trees[length].GetName() << std::endl << std::endl;
    //trees[length].Branch(bname.c_str(), &bCountingValues[bname]);
    std::cout << "Located in tree " << trees[length]->GetName() << std::endl << std::endl;
    trees[length]->Branch(bname.c_str(), &bCountingValues[bname]);
  }
  for (std::map<std::string, std::map<long long, std::map<long long, bool> > >::iterator outer = fBoolIntIntMap.begin();
      outer != fBoolIntIntMap.end(); ++outer) {
    std::string bname = outer->first;
    unsigned length = outer->second.size();
    std::cout << "Creating branch " << bname << " of type vector<bool>" << std::endl;
    //std::cout << "Located in tree " << trees[length].GetName() << std::endl << std::endl;
    //trees[length].Branch(bname.c_str(), &bBoolIntValues[bname]);
    std::cout << "Located in tree " << trees[length]->GetName() << std::endl << std::endl;
    trees[length]->Branch(bname.c_str(), &bBoolIntValues[bname]);
  }
  for (std::map<std::string, std::map<long long, std::map<long long, long double> > >::iterator outer = fDecimalIntIntMap.begin();
      outer != fDecimalIntIntMap.end(); ++outer) {
    std::string bname = outer->first;
    unsigned length = outer->second.size();
    std::cout << "Creating branch " << bname << " of type vector<double>" << std::endl;
    //std::cout << "Located in tree " << trees[length].GetName() << std::endl << std::endl;
    //trees[length].Branch(bname.c_str(), &bDecimalIntValues[bname]);
    std::cout << "Located in tree " << trees[length]->GetName() << std::endl << std::endl;
    trees[length]->Branch(bname.c_str(), &bDecimalIntValues[bname]);
  }
  for (std::map<std::string, std::map<long long, std::map<long long, long long> > >::iterator outer = fIntegerIntIntMap.begin();
      outer != fIntegerIntIntMap.end(); ++outer) {
    std::string bname = outer->first;
    unsigned length = outer->second.size();
    std::cout << "Creating branch " << bname << " of type vector<long long>" << std::endl;
    //std::cout << "Located in tree " << trees[length].GetName() << std::endl << std::endl;
    //trees[length].Branch(bname.c_str(), &bIntegerIntValues[bname]);
    std::cout << "Located in tree " << trees[length]->GetName() << std::endl << std::endl;
    trees[length]->Branch(bname.c_str(), &bIntegerIntValues[bname]);
  }
  for (std::map<std::string, std::map<long long, std::map<long long, unsigned long long> > >::iterator outer = fCountingIntIntMap.begin();
      outer != fCountingIntIntMap.end(); ++outer) {
    std::string bname = outer->first;
    unsigned length = outer->second.size();
    std::cout << "Creating branch " << bname << " of type vector<unsigned long long>" << std::endl;
    //std::cout << "Located in tree " << trees[length].GetName() << std::endl << std::endl;
    //trees[length].Branch(bname.c_str(), &bCountingIntValues[bname]);
    std::cout << "Located in tree " << trees[length]->GetName() << std::endl << std::endl;
    trees[length]->Branch(bname.c_str(), &bCountingIntValues[bname]);
  }

  // actual loop to fill in the TTree
  for (long long i = 1; i <= fCount; ++i) {
    for (std::map<std::string, std::map<long long, bool> >::iterator outer = fBoolIntMap.begin();
        outer != fBoolIntMap.end(); ++outer) {
      std::string bname = outer->first;
      unsigned length = outer->second.size();
      if (Exists(bname, i)) {
        fill[length] = true;
        //trees[length].SetBranchStatus(bname.c_str(), 1);
        trees[length]->SetBranchStatus(bname.c_str(), 1);
        bBoolValues[bname] = GetBool(bname, i);
      }
      else
        trees[length]->SetBranchStatus(bname.c_str(), 0);
        //trees[length].SetBranchStatus(bname.c_str(), 0);
    }
    for (std::map<std::string, std::map<long long, long double> >::iterator outer = fDecimalIntMap.begin();
        outer != fDecimalIntMap.end(); ++outer) {
      std::string bname = outer->first;
      unsigned length = outer->second.size();
      if (Exists(bname, i)) {
        fill[length] = true;
        trees[length]->SetBranchStatus(bname.c_str(), 1);
        //trees[length].SetBranchStatus(bname.c_str(), 1);
        bDecimalValues[bname] = GetDecimal(bname, i);
      }
      else
        trees[length]->SetBranchStatus(bname.c_str(), 0);
        //trees[length].SetBranchStatus(bname.c_str(), 0);
    }
    for (std::map<std::string, std::map<long long, long long> >::iterator outer = fIntegerIntMap.begin();
        outer != fIntegerIntMap.end(); ++outer) {
      std::string bname = outer->first;
      unsigned length = outer->second.size();
      if (Exists(bname, i)) {
        fill[length] = true;
        trees[length]->SetBranchStatus(bname.c_str(), 1);
        //trees[length].SetBranchStatus(bname.c_str(), 1);
        bIntegerValues[bname] = GetInteger(bname, i);
      }
      else
        trees[length]->SetBranchStatus(bname.c_str(), 0);
        //trees[length].SetBranchStatus(bname.c_str(), 0);
    }
    for (std::map<std::string, std::map<long long, unsigned long long> >::iterator outer = fCountingIntMap.begin();
        outer != fCountingIntMap.end(); ++outer) {
      std::string bname = outer->first;
      unsigned length = outer->second.size();
      if (Exists(bname, i)) {
        fill[length] = true;
        trees[length]->SetBranchStatus(bname.c_str(), 1);
        //trees[length].SetBranchStatus(bname.c_str(), 1);
        bCountingValues[bname] = GetCounting(bname, i);
      }
      else
        trees[length]->SetBranchStatus(bname.c_str(), 0);
        //trees[length].SetBranchStatus(bname.c_str(), 0);
    }
    for (std::map<std::string, std::map<long long, std::map<long long, bool> > >::iterator outer = fBoolIntIntMap.begin();
        outer != fBoolIntIntMap.end(); ++outer) {
      std::string bname = outer->first;
      unsigned length = outer->second.size();
      if (Exists(bname, i)) {
        fill[length] = true;
        std::vector<bool> temp;
        for (std::map<long long, bool>::iterator it = outer->second[i].begin();
             it != outer->second[i].end(); ++it)
          temp.push_back(it->second);
        trees[length]->SetBranchStatus(bname.c_str(), 1);
        //trees[length].SetBranchStatus(bname.c_str(), 1);
        bBoolIntValues[bname] = temp;
      }
      else
        trees[length]->SetBranchStatus(bname.c_str(), 0);
        //trees[length].SetBranchStatus(bname.c_str(), 0);
    }
    for (std::map<std::string, std::map<long long, std::map<long long, long double> > >::iterator outer = fDecimalIntIntMap.begin();
        outer != fDecimalIntIntMap.end(); ++outer) {
      std::string bname = outer->first;
      unsigned length = outer->second.size();
      if (Exists(bname, i)) {
        fill[length] = true;
        std::vector<double> temp;
        for (std::map<long long, long double>::iterator it = outer->second[i].begin();
             it != outer->second[i].end(); ++it)
          temp.push_back(it->second);
        trees[length]->SetBranchStatus(bname.c_str(), 1);
        //trees[length].SetBranchStatus(bname.c_str(), 1);
        bDecimalIntValues[bname] = temp;
      }
      else
        trees[length]->SetBranchStatus(bname.c_str(), 0);
        //trees[length].SetBranchStatus(bname.c_str(), 0);
    }
    for (std::map<std::string, std::map<long long, std::map<long long, long long> > >::iterator outer = fIntegerIntIntMap.begin();
        outer != fIntegerIntIntMap.end(); ++outer) {
      std::string bname = outer->first;
      unsigned length = outer->second.size();
      if (Exists(bname, i)) {
        fill[length] = true;
        std::vector<long long> temp;
        for (std::map<long long, long long>::iterator it = outer->second[i].begin();
             it != outer->second[i].end(); ++it)
          temp.push_back(it->second);
        trees[length]->SetBranchStatus(bname.c_str(), 1);
        //trees[length].SetBranchStatus(bname.c_str(), 1);
        bIntegerIntValues[bname] = temp;
      }
      else
        trees[length]->SetBranchStatus(bname.c_str(), 0);
        //trees[length].SetBranchStatus(bname.c_str(), 0);
    }
    for (std::map<std::string, std::map<long long, std::map<long long, unsigned long long> > >::iterator outer = fCountingIntIntMap.begin();
        outer != fCountingIntIntMap.end(); ++outer) {
      std::string bname = outer->first;
      unsigned length = outer->second.size();
      if (Exists(bname, i)) {
        fill[length] = true;
        std::vector<unsigned long long> temp;
        for (std::map<long long, unsigned long long>::iterator it = outer->second[i].begin();
             it != outer->second[i].end(); ++it)
          temp.push_back(it->second);
        trees[length]->SetBranchStatus(bname.c_str(), 1);
        //trees[length].SetBranchStatus(bname.c_str(), 1);
        bCountingIntValues[bname] = temp;
      }
      else
        trees[length]->SetBranchStatus(bname.c_str(), 0);
        //trees[length].SetBranchStatus(bname.c_str(), 0);
    }

    // fill trees and reset fill flags
    for (std::map<unsigned, bool>::iterator it = fill.begin(); it != fill.end(); ++it) {
      if (it->second)
        trees[it->first]->Fill();
        //trees[it->first].Fill();
      it->second = false;
    }
  }

  for (std::map<unsigned, TTree*>::iterator it = trees.begin(); it != trees.end(); ++it)
  //for (std::map<unsigned, TTree>::iterator it = trees.begin(); it != trees.end(); ++it)
    it->second->SetBranchStatus("*", kTRUE);
    //it->second.SetBranchStatus("*", kTRUE);
  f.Write();
  f.Close();
}

} /* HAL */ 

