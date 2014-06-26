#include <HAL/AnalysisTreeWriter.h>
#include <string>
#include <iostream>
#include <vector>
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
#include <aux/boost/foreach.hpp>
#endif
#include <TFile.h>
#include <TTree.h>
#include <HAL/Exceptions.h>

ClassImp(HAL::AnalysisTreeWriter);

namespace HAL
{

//______________________________________________________________________________
AnalysisTreeWriter::AnalysisTreeWriter (const TString &ofile) : 
  fCount(0), fOutputFileName(ofile) 
{
}

//______________________________________________________________________________
void  AnalysisTreeWriter::SetValue (const TString &n, const bool &v) 
{
  fTreeIndicesMap[fBranchTreeMap[n].EqualTo("") ? fTreeName : fBranchTreeMap[n]].insert(fCount);
  AnalysisData::SetValue(n, v, fCount);
}

//______________________________________________________________________________
void  AnalysisTreeWriter::SetValue (const TString &n, const bool &v, 
                                    const long long &i) 
{
  fTreeIndicesMap[fBranchTreeMap[n].EqualTo("") ? fTreeName : fBranchTreeMap[n]].insert(fCount);
  AnalysisData::SetValue(n, v, fCount, i);
}

//______________________________________________________________________________
void  AnalysisTreeWriter::SetValue (const TString &n, const long double &v) 
{
  fTreeIndicesMap[fBranchTreeMap[n].EqualTo("") ? fTreeName : fBranchTreeMap[n]].insert(fCount);
  AnalysisData::SetValue(n, v, fCount);
}

//______________________________________________________________________________
void  AnalysisTreeWriter::SetValue (const TString &n, const long double &v, 
                                    const long long &i) 
{
  fTreeIndicesMap[fBranchTreeMap[n].EqualTo("") ? fTreeName : fBranchTreeMap[n]].insert(fCount);
  AnalysisData::SetValue(n, v, fCount, i);
}

//______________________________________________________________________________
void  AnalysisTreeWriter::SetValue (const TString &n, const long long &v) 
{
  fTreeIndicesMap[fBranchTreeMap[n].EqualTo("") ? fTreeName : fBranchTreeMap[n]].insert(fCount);
  AnalysisData::SetValue(n, v, fCount);
}

//______________________________________________________________________________
void AnalysisTreeWriter::SetValue (const TString &n, const long long &v, 
                                   const long long &i) 
{
  fTreeIndicesMap[fBranchTreeMap[n].EqualTo("") ? fTreeName : fBranchTreeMap[n]].insert(fCount);
  AnalysisData::SetValue(n, v, fCount, i);
}

//______________________________________________________________________________
void  AnalysisTreeWriter::SetValue (const TString &n, const unsigned long long &v) 
{
  fTreeIndicesMap[fBranchTreeMap[n].EqualTo("") ? fTreeName : fBranchTreeMap[n]].insert(fCount);
  AnalysisData::SetValue(n, v, fCount);
}

//______________________________________________________________________________
void  AnalysisTreeWriter::SetValue (const TString &n, const unsigned long long &v, 
                                    const long long &i) 
{
  fTreeIndicesMap[fBranchTreeMap[n].EqualTo("") ? fTreeName : fBranchTreeMap[n]].insert(fCount);
  AnalysisData::SetValue(n, v, fCount, i);
}

//______________________________________________________________________________
void AnalysisTreeWriter::WriteData() 
{
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
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  std::pair<std::string, std::map<long long, bool> > pair_BIM;
  BOOST_FOREACH( pair_BIM, fBoolIntMap )
#else
  for (auto pair_BIM: fBoolIntMap)
#endif
  {
    TString treeName = fBranchTreeMap[pair_BIM.first.c_str()].EqualTo("") ? fTreeName : 
      fBranchTreeMap[pair_BIM.first.c_str()];
    if (trees.count(treeName) == 0) {
      trees[treeName] = new TTree(treeName, fTreeDescription.Data());
      fill[treeName] = false;
    }
  }
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  std::pair<std::string, std::map<long long, long double> > pair_DIM;
  BOOST_FOREACH( pair_DIM, fDecimalIntMap )
#else
  for (auto pair_DIM: fDecimalIntMap)
#endif
  {
    TString treeName = fBranchTreeMap[pair_DIM.first.c_str()].EqualTo("") ? fTreeName : 
      fBranchTreeMap[pair_DIM.first.c_str()];
    if (trees.count(treeName) == 0) {
      trees[treeName] = new TTree(treeName, fTreeDescription.Data());
      fill[treeName] = false;
    }
  }
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  std::pair<std::string, std::map<long long, long long> > pair_IIM;
  BOOST_FOREACH( pair_IIM, fIntegerIntMap )
#else
  for (auto pair_IIM: fIntegerIntMap)
#endif
  {
    TString treeName = fBranchTreeMap[pair_IIM.first.c_str()].EqualTo("") ? fTreeName : 
      fBranchTreeMap[pair_IIM.first.c_str()];
    if (trees.count(treeName) == 0) {
      trees[treeName] = new TTree(treeName, fTreeDescription.Data());
      fill[treeName] = false;
    }
  }
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  std::pair<std::string, std::map<long long, unsigned long long> > pair_CIM;
  BOOST_FOREACH( pair_CIM, fCountingIntMap )
#else
  for (auto pair_CIM: fCountingIntMap)
#endif
  {
    TString treeName = fBranchTreeMap[pair_CIM.first.c_str()].EqualTo("") ? fTreeName : 
      fBranchTreeMap[pair_CIM.first.c_str()];
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
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  std::pair<std::string, std::map<long long, std::map<long long, bool> > > pair_BIIM;
  BOOST_FOREACH( pair_BIIM, fBoolIntIntMap )
#else
  for (auto pair_BIIM: fBoolIntIntMap)
#endif
  {
    TString treeName = fBranchTreeMap[pair_BIIM.first.c_str()].EqualTo("") ? fTreeName : 
      fBranchTreeMap[pair_BIIM.first.c_str()];
    if (trees.count(treeName) == 0) {
      trees[treeName] = new TTree(treeName, fTreeDescription.Data());
      fill[treeName] = false;
    }
  }
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  std::pair<std::string, std::map<long long, std::map<long long, long double> > > pair_DIIM;
  BOOST_FOREACH( pair_DIIM, fDecimalIntIntMap )
#else
  for (auto pair_DIIM: fDecimalIntIntMap)
#endif
  {
    TString treeName = fBranchTreeMap[pair_DIIM.first.c_str()].EqualTo("") ? fTreeName : 
      fBranchTreeMap[pair_DIIM.first.c_str()];
    if (trees.count(treeName) == 0) {
      trees[treeName] = new TTree(treeName, fTreeDescription.Data());
      fill[treeName] = false;
    }
  }
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  std::pair<std::string, std::map<long long, std::map<long long, long long> > > pair_IIIM;
  BOOST_FOREACH( pair_IIIM, fIntegerIntIntMap )
#else
  for (auto pair_IIIM: fIntegerIntIntMap)
#endif
  {
    TString treeName = fBranchTreeMap[pair_IIIM.first.c_str()].EqualTo("") ? fTreeName : 
      fBranchTreeMap[pair_IIIM.first.c_str()];
    if (trees.count(treeName) == 0) {
      trees[treeName] = new TTree(treeName, fTreeDescription.Data());
      fill[treeName] = false;
    }
  }
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  std::pair<std::string, std::map<long long, std::map<long long, unsigned long long> > > pair_CIIM;
  BOOST_FOREACH( pair_CIIM, fCountingIntIntMap )
#else
  for (auto pair_CIIM: fCountingIntIntMap)
#endif
  {
    TString treeName = fBranchTreeMap[pair_CIIM.first.c_str()].EqualTo("") ? fTreeName : 
      fBranchTreeMap[pair_CIIM.first.c_str()];
    if (trees.count(treeName) == 0) {
      trees[treeName] = new TTree(treeName, fTreeDescription.Data());
      fill[treeName] = false;
    }
  }

  // Loop over maps - each key is a branch name
  std::cout << std::endl;
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  BOOST_FOREACH( pair_BIM, fBoolIntMap )
#else
  for (auto pair_BIM: fBoolIntMap)
#endif
  {
    std::string bname = pair_BIM.first;
    TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
    std::cout << "Creating branch " << bname << " of type bool" << std::endl;
    std::cout << "Located in tree " << trees[tname]->GetName() << std::endl << std::endl;
    trees[tname]->Branch(bname.c_str(), &bBoolValues[bname]);
  }
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  BOOST_FOREACH( pair_DIM, fDecimalIntMap )
#else
  for (auto pair_DIM: fDecimalIntMap)
#endif
  {
    std::string bname = pair_DIM.first;
    TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
    std::cout << "Creating branch " << bname << " of type double" << std::endl;
    std::cout << "Located in tree " << trees[tname]->GetName() << std::endl << std::endl;
    trees[tname]->Branch(bname.c_str(), &bDecimalValues[bname]);
  }
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  BOOST_FOREACH( pair_IIM, fIntegerIntMap )
#else
  for (auto pair_IIM: fIntegerIntMap)
#endif
  {
    std::string bname = pair_IIM.first;
    TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
    std::cout << "Creating branch " << bname << " of type long long" << std::endl;
    std::cout << "Located in tree " << trees[tname]->GetName() << std::endl << std::endl;
    trees[tname]->Branch(bname.c_str(), &bIntegerValues[bname]);
  }
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  BOOST_FOREACH( pair_CIM, fCountingIntMap )
#else
  for (auto pair_CIM: fCountingIntMap)
#endif
  {
    std::string bname = pair_CIM.first;
    TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
    std::cout << "Creating branch " << bname << " of type unsigned long long" << std::endl;
    std::cout << "Located in tree " << trees[tname]->GetName() << std::endl << std::endl;
    trees[tname]->Branch(bname.c_str(), &bCountingValues[bname]);
  }
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  BOOST_FOREACH( pair_BIIM, fBoolIntIntMap )
#else
  for (auto pair_BIIM: fBoolIntIntMap)
#endif
  {
    std::string bname = pair_BIIM.first;
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
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  BOOST_FOREACH( pair_DIIM, fDecimalIntIntMap )
#else
  for (auto pair_DIIM: fDecimalIntIntMap)
#endif
  {
    std::string bname = pair_DIIM.first;
    TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
    std::cout << "Creating branch " << bname << " of type vector<double>" << std::endl;
    std::cout << "Located in tree " << trees[tname]->GetName() << std::endl << std::endl;
    trees[tname]->Branch(bname.c_str(), &bDecimalIntValues[bname]);
  }
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  BOOST_FOREACH( pair_IIIM, fIntegerIntIntMap )
#else
  for (auto pair_IIIM: fIntegerIntIntMap)
#endif
  {
    std::string bname = pair_IIIM.first;
    TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
    std::cout << "Creating branch " << bname << " of type vector<long long>" << std::endl;
    std::cout << "Located in tree " << trees[tname]->GetName() << std::endl << std::endl;
    trees[tname]->Branch(bname.c_str(), &bIntegerIntValues[bname]);
  }
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  BOOST_FOREACH( pair_CIIM, fCountingIntIntMap )
#else
  for (auto pair_CIIM: fCountingIntIntMap)
#endif
  {
    std::string bname = pair_CIIM.first;
    TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
    std::cout << "Creating branch " << bname << " of type vector<unsigned long long>" << std::endl;
    std::cout << "Located in tree " << trees[tname]->GetName() << std::endl << std::endl;
    trees[tname]->Branch(bname.c_str(), &bCountingIntValues[bname]);
  }

//  // actual loop to fill in the TTree
//  for (long long i = 1; i <= fCount; ++i) {
//#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
//    BOOST_FOREACH( pair_BIM, fBoolIntMap )
//#else
//    for (auto pair_BIM: fBoolIntMap)
//#endif
//    {
//      std::string bname = pair_BIM.first;
//      TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
//      if (fTreeIndicesMap[tname].count(i) != 0) {
//        fill[tname] = true;
//        trees[tname]->SetBranchStatus(bname.c_str(), 1);
//        if (Exists(bname, i))
//          bBoolValues[bname] = GetBool(bname, i);
//        else // DEFAULT
//          bBoolValues[bname] = false;
//      }
//      else
//        trees[tname]->SetBranchStatus(bname.c_str(), 0);
//    }
//#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
//    BOOST_FOREACH( pair_DIM, fDecimalIntMap )
//#else
//    for (auto pair_DIM: fDecimalIntMap)
//#endif
//    {
//      std::string bname = pair_DIM.first;
//      TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
//      if (fTreeIndicesMap[tname].count(i) != 0) {
//        fill[tname] = true;
//        trees[tname]->SetBranchStatus(bname.c_str(), 1);
//        if (Exists(bname, i))
//          bDecimalValues[bname] = GetDecimal(bname, i);
//        else // DEFAULT
//          bDecimalValues[bname] = -536870912.0;
//      }
//      else
//        trees[tname]->SetBranchStatus(bname.c_str(), 0);
//    }
//#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
//    BOOST_FOREACH( pair_IIM, fIntegerIntMap )
//#else
//    for (auto pair_IIM: fIntegerIntMap)
//#endif
//    {
//      std::string bname = pair_IIM.first;
//      TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
//      if (fTreeIndicesMap[tname].count(i) != 0) {
//        fill[tname] = true;
//        trees[tname]->SetBranchStatus(bname.c_str(), 1);
//        if (Exists(bname, i))
//          bIntegerValues[bname] = GetInteger(bname, i);
//        else // DEFAULT
//          bIntegerValues[bname] = -536870912;
//      }
//      else
//        trees[tname]->SetBranchStatus(bname.c_str(), 0);
//    }
//#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
//    BOOST_FOREACH( pair_CIM, fCountingIntMap )
//#else
//    for (auto pair_CIM: fCountingIntMap)
//#endif
//    {
//      std::string bname = pair_CIM.first;
//      TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
//      if (fTreeIndicesMap[tname].count(i) != 0) {
//        fill[tname] = true;
//        trees[tname]->SetBranchStatus(bname.c_str(), 1);
//        if (Exists(bname, i))
//          bCountingValues[bname] = GetCounting(bname, i);
//        else // DEFAULT
//          bCountingValues[bname] = 536870912;
//      }
//      else
//        trees[tname]->SetBranchStatus(bname.c_str(), 0);
//    }
//#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
//    BOOST_FOREACH( pair_BIIM, fBoolIntIntMap )
//#else
//    for (auto pair_BIIM: fBoolIntIntMap)
//#endif
//    {
//      std::string bname = pair_BIIM.first;
//      TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
//      if (fTreeIndicesMap[tname].count(i) != 0) {
//        std::vector<bool> temp;
//        fill[tname] = true;
//        trees[tname]->SetBranchStatus(bname.c_str(), 1);
//        if (Exists(bname, i)) {
//#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
//          std::pair<long long, bool> it;
//          BOOST_FOREACH( it, pair_BIIM.second[i] )
//#else
//          for (auto it: pair_BIIM.second[i])
//#endif
//            temp.push_back(it.second);
//        }
//        bBoolIntValues[bname] = temp;
//      }
//      else
//        trees[tname]->SetBranchStatus(bname.c_str(), 0);
//    }
//#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
//    BOOST_FOREACH( pair_DIIM, fDecimalIntIntMap )
//#else
//    for (auto pair_DIIM: fDecimalIntIntMap)
//#endif
//    {
//      std::string bname = pair_DIIM.first;
//      TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
//      if (fTreeIndicesMap[tname].count(i) != 0) {
//        std::vector<double> temp;
//        fill[tname] = true;
//        trees[tname]->SetBranchStatus(bname.c_str(), 1);
//        if (Exists(bname, i)) {
//#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
//          std::pair<long long, long double> it;
//          BOOST_FOREACH( it, pair_DIIM.second[i] )
//#else
//          for (auto it: pair_DIIM.second[i])
//#endif
//            temp.push_back(it.second);
//        }
//        bDecimalIntValues[bname] = temp; 
//      }
//      else
//        trees[tname]->SetBranchStatus(bname.c_str(), 0);
//    }
//#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
//    BOOST_FOREACH( pair_IIIM, fIntegerIntIntMap )
//#else
//    for (auto pair_IIIM: fIntegerIntIntMap)
//#endif
//    {
//      std::string bname = pair_IIIM.first;
//      TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
//      if (fTreeIndicesMap[tname].count(i) != 0) {
//        std::vector<long long> temp;
//        fill[tname] = true;
//        trees[tname]->SetBranchStatus(bname.c_str(), 1);
//        if (Exists(bname, i)) {
//#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
//          std::pair<long long, long long> it;
//          BOOST_FOREACH( it, pair_IIIM.second[i] )
//#else
//          for (auto it: pair_IIIM.second[i])
//#endif
//            temp.push_back(it.second);
//        }
//        bIntegerIntValues[bname] = temp;
//      }
//      else
//        trees[tname]->SetBranchStatus(bname.c_str(), 0);
//    }
//#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
//    BOOST_FOREACH( pair_CIIM, fCountingIntIntMap )
//#else
//    for (auto pair_CIIM: fCountingIntIntMap)
//#endif
//    {
//      std::string bname = pair_CIIM.first;
//      TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
//      if (fTreeIndicesMap[tname].count(i) != 0) {
//        std::vector<unsigned long long> temp;
//        fill[tname] = true;
//        trees[tname]->SetBranchStatus(bname.c_str(), 1);
//        if (Exists(bname, i)) {
//#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
//          std::pair<long long, unsigned long long> it;
//          BOOST_FOREACH( it, pair_CIIM.second[i] )
//#else
//          for (auto it: pair_CIIM.second[i])
//#endif
//            temp.push_back(it.second);
//        }
//        bCountingIntValues[bname] = temp;
//      }
//      else
//        trees[tname]->SetBranchStatus(bname.c_str(), 0);
//    }
//
//    // fill trees and reset fill flags
//#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
//    std::pair<TString, bool> pair_fill;
//    BOOST_FOREACH( pair_fill, fill )
//#else
//    for (auto pair_fill: fill)
//#endif
//    {
//      if (pair_fill.second)
//        trees[pair_fill.first]->Fill();
//      pair_fill.second = false;
//    }
//
//    // Write message
//    if ((i + 1) % 100 == 0) {
//      std::cout.flush();
//      std::cout << "\r" << "Writing event: " << i + 1;
//    }
//  }
//
//#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
//  std::pair<TString, TTree*> pair_trees;
//  BOOST_FOREACH( pair_trees, trees )
//#else
//  for (auto pair_trees: trees)
//#endif
//    pair_trees.second->SetBranchStatus("*", kTRUE);
//  f.Write();
//  f.Close();


//  // Loop over maps to record how many trees to create
//  for (std::map<std::string, std::map<long long, bool>, internal::string_cmp>::iterator outer = fBoolIntMap.begin();
//      outer != fBoolIntMap.end(); ++outer) {
//    TString treeName = fBranchTreeMap[outer->first.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[outer->first.c_str()];
//    if (trees.count(treeName) == 0) {
//      trees[treeName] = new TTree(treeName, fTreeDescription.Data());
//      fill[treeName] = false;
//    }
//  }
//  for (std::map<std::string, std::map<long long, long double>, internal::string_cmp >::iterator outer = fDecimalIntMap.begin();
//      outer != fDecimalIntMap.end(); ++outer) {
//    TString treeName = fBranchTreeMap[outer->first.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[outer->first.c_str()];
//    if (trees.count(treeName) == 0) {
//      trees[treeName] = new TTree(treeName, fTreeDescription.Data());
//      fill[treeName] = false;
//    }
//  }
//  for (std::map<std::string, std::map<long long, long long>, internal::string_cmp >::iterator outer = fIntegerIntMap.begin();
//      outer != fIntegerIntMap.end(); ++outer) {
//    TString treeName = fBranchTreeMap[outer->first.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[outer->first.c_str()];
//    if (trees.count(treeName) == 0) {
//      trees[treeName] = new TTree(treeName, fTreeDescription.Data());
//      fill[treeName] = false;
//    }
//  }
//  for (std::map<std::string, std::map<long long, unsigned long long>, internal::string_cmp >::iterator outer = fCountingIntMap.begin();
//      outer != fCountingIntMap.end(); ++outer) {
//    TString treeName = fBranchTreeMap[outer->first.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[outer->first.c_str()];
//    if (trees.count(treeName) == 0) {
//      trees[treeName] = new TTree(treeName, fTreeDescription.Data());
//      fill[treeName] = false;
//    }
//    //unsigned length = outer->second.size();
//    //if (length > max_scalar_nentries) max_scalar_nentries = length;
//    //TString treeName = fBranchTreeMap[outer->first].EqualTo("") ? fTreeName : fBranchTreeMap[outer->first];//fTreeName;
//    //if (trees.count(length) == 0) {
//    //  if (!trees.empty())
//    //    treeName.Append(TString::Format("%u", ++treeCount));
//    //  trees[length] = new TTree(treeName, fTreeDescription.Data());
//    //  fill[length] = false;
//    //}
//  }
//  for (std::map<std::string, std::map<long long, std::map<long long, bool> >, internal::string_cmp >::iterator outer = fBoolIntIntMap.begin();
//      outer != fBoolIntIntMap.end(); ++outer) {
//    TString treeName = fBranchTreeMap[outer->first.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[outer->first.c_str()];
//    if (trees.count(treeName) == 0) {
//      trees[treeName] = new TTree(treeName, fTreeDescription.Data());
//      fill[treeName] = false;
//    }
//  }
//  for (std::map<std::string, std::map<long long, std::map<long long, long double> >, internal::string_cmp >::iterator outer = fDecimalIntIntMap.begin();
//      outer != fDecimalIntIntMap.end(); ++outer) {
//    TString treeName = fBranchTreeMap[outer->first.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[outer->first.c_str()];
//    if (trees.count(treeName) == 0) {
//      trees[treeName] = new TTree(treeName, fTreeDescription.Data());
//      fill[treeName] = false;
//    }
//  }
//  for (std::map<std::string, std::map<long long, std::map<long long, long long> >, internal::string_cmp >::iterator outer = fIntegerIntIntMap.begin();
//      outer != fIntegerIntIntMap.end(); ++outer) {
//    TString treeName = fBranchTreeMap[outer->first.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[outer->first.c_str()];
//    if (trees.count(treeName) == 0) {
//      trees[treeName] = new TTree(treeName, fTreeDescription.Data());
//      fill[treeName] = false;
//    }
//  }
//  for (std::map<std::string, std::map<long long, std::map<long long, unsigned long long> >, internal::string_cmp >::iterator outer = fCountingIntIntMap.begin();
//      outer != fCountingIntIntMap.end(); ++outer) {
//    TString treeName = fBranchTreeMap[outer->first.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[outer->first.c_str()];
//    if (trees.count(treeName) == 0) {
//      trees[treeName] = new TTree(treeName, fTreeDescription.Data());
//      fill[treeName] = false;
//    }
//  }
//
//  // Loop over maps - each key is a branch name
//  std::cout << std::endl;
//  for (std::map<std::string, std::map<long long, bool>, internal::string_cmp >::iterator outer = fBoolIntMap.begin();
//      outer != fBoolIntMap.end(); ++outer) {
//    std::string bname = outer->first;
//    TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
//    std::cout << "Creating branch " << bname << " of type bool" << std::endl;
//    std::cout << "Located in tree " << trees[tname]->GetName() << std::endl << std::endl;
//    trees[tname]->Branch(bname.c_str(), &bBoolValues[bname]);
//  }
//  for (std::map<std::string, std::map<long long, long double>, internal::string_cmp >::iterator outer = fDecimalIntMap.begin();
//      outer != fDecimalIntMap.end(); ++outer) {
//    std::string bname = outer->first;
//    TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
//    std::cout << "Creating branch " << bname << " of type double" << std::endl;
//    std::cout << "Located in tree " << trees[tname]->GetName() << std::endl << std::endl;
//    trees[tname]->Branch(bname.c_str(), &bDecimalValues[bname]);
//  }
//  for (std::map<std::string, std::map<long long, long long>, internal::string_cmp >::iterator outer = fIntegerIntMap.begin();
//      outer != fIntegerIntMap.end(); ++outer) {
//    std::string bname = outer->first;
//    TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
//    std::cout << "Creating branch " << bname << " of type long long" << std::endl;
//    std::cout << "Located in tree " << trees[tname]->GetName() << std::endl << std::endl;
//    trees[tname]->Branch(bname.c_str(), &bIntegerValues[bname]);
//  }
//  for (std::map<std::string, std::map<long long, unsigned long long>, internal::string_cmp >::iterator outer = fCountingIntMap.begin();
//      outer != fCountingIntMap.end(); ++outer) {
//    std::string bname = outer->first;
//    TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
//    std::cout << "Creating branch " << bname << " of type unsigned long long" << std::endl;
//    std::cout << "Located in tree " << trees[tname]->GetName() << std::endl << std::endl;
//    trees[tname]->Branch(bname.c_str(), &bCountingValues[bname]);
//  }
//  for (std::map<std::string, std::map<long long, std::map<long long, bool> >, internal::string_cmp >::iterator outer = fBoolIntIntMap.begin();
//      outer != fBoolIntIntMap.end(); ++outer) {
//    std::string bname = outer->first;
//    TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
//    //unsigned length = outer->second.size();
//    std::cout << "Creating branch " << bname << " of type vector<bool>" << std::endl;
//    //std::cout << "Located in tree " << trees[max_scalar_nentries]->GetName() << std::endl << std::endl;
//    std::cout << "Located in tree " << trees[tname]->GetName() << std::endl << std::endl;
//    //std::cout << "Located in tree " << trees[length]->GetName() << std::endl << std::endl;
//    trees[tname]->Branch(bname.c_str(), &bBoolIntValues[bname]);
//    //trees[max_scalar_nentries]->Branch(bname.c_str(), &bBoolIntValues[bname]);
//    //trees[length]->Branch(bname.c_str(), &bBoolIntValues[bname]);
//  }
//  for (std::map<std::string, std::map<long long, std::map<long long, long double> >, internal::string_cmp >::iterator outer = fDecimalIntIntMap.begin();
//      outer != fDecimalIntIntMap.end(); ++outer) {
//    std::string bname = outer->first;
//    TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
//    std::cout << "Creating branch " << bname << " of type vector<double>" << std::endl;
//    std::cout << "Located in tree " << trees[tname]->GetName() << std::endl << std::endl;
//    trees[tname]->Branch(bname.c_str(), &bDecimalIntValues[bname]);
//  }
//  for (std::map<std::string, std::map<long long, std::map<long long, long long> >, internal::string_cmp >::iterator outer = fIntegerIntIntMap.begin();
//      outer != fIntegerIntIntMap.end(); ++outer) {
//    std::string bname = outer->first;
//    TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
//    std::cout << "Creating branch " << bname << " of type vector<long long>" << std::endl;
//    std::cout << "Located in tree " << trees[tname]->GetName() << std::endl << std::endl;
//    trees[tname]->Branch(bname.c_str(), &bIntegerIntValues[bname]);
//  }
//  for (std::map<std::string, std::map<long long, std::map<long long, unsigned long long> >, internal::string_cmp >::iterator outer = fCountingIntIntMap.begin();
//      outer != fCountingIntIntMap.end(); ++outer) {
//    std::string bname = outer->first;
//    TString tname = fBranchTreeMap[bname.c_str()].EqualTo("") ? fTreeName : fBranchTreeMap[bname.c_str()];
//    std::cout << "Creating branch " << bname << " of type vector<unsigned long long>" << std::endl;
//    std::cout << "Located in tree " << trees[tname]->GetName() << std::endl << std::endl;
//    trees[tname]->Branch(bname.c_str(), &bCountingIntValues[bname]);
//  }

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
    
    // Write message
    if ((i + 1) % 100 == 0) {
      std::cout.flush();
      std::cout << "\r" << "Writing event: " << i + 1;
    }
  }

  for (std::map<TString, TTree*>::iterator it = trees.begin(); it != trees.end(); ++it)
    it->second->SetBranchStatus("*", kTRUE);
  f.Write();
  f.Close();
}

} /* HAL */ 

