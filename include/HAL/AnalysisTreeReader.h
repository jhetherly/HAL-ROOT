#include <TROOT.h>
#include <TTree.h>
#include <TBranch.h>
#include <TString.h>
#include <TNamed.h>
#include <vector>

#ifndef HAL_ANALYSIS_TREE_READER
#define HAL_ANALYSIS_TREE_READER

namespace HAL {

class AnalysisTreeReader : public TNamed {
public:
  AnalysisTreeReader (TTree *tree = 0);
  virtual ~AnalysisTreeReader ();
  void SetTree (TTree *tree) {fChain = tree;}
  void SetEntry (Long64_t entry) {fEntry = entry;}

  //Bool_t                  getBool (TString branchname, Int_t row_i = -1, Int_t column_i = -1);
  //Byte_t                  getByte (TString branchname, Int_t row_i = -1, Int_t column_i = -1);
  //Int_t                   getInt (TString branchname, Int_t row_i = -1, Int_t column_i = -1);
  //UInt_t                  getUInt (TString branchname, Int_t row_i = -1, Int_t column_i = -1);
  //Short_t                 getShort (TString branchname, Int_t row_i = -1, Int_t column_i = -1);
  //UShort_t                getUShort (TString branchname, Int_t row_i = -1, Int_t column_i = -1);
  //Long64_t                getLong (TString branchname, Int_t row_i = -1, Int_t column_i = -1);
  //long long int           getLongLongInt (TString branchname, Int_t row_i = -1, Int_t column_i = -1);
  //ULong64_t               getULong (TString branchname, Int_t row_i = -1, Int_t column_i = -1);
  //unsigned long long int  getULongLong (TString branchname, Int_t row_i = -1, Int_t column_i = -1);
  //Float16_t               getFloat (TString branchname, Int_t row_i = -1, Int_t column_i = -1);
  //Double32_t              getDouble (TString branchname, Int_t row_i = -1, Int_t column_i = -1);
  //long double             getLongDouble (TString branchname, Int_t row_i = -1, Int_t column_i = -1);
  //Char_t                  getChar (TString branchname, Int_t row_i = -1, Int_t column_i = -1);
  //TString                 getTString (TString branchname, Int_t row_i = -1, Int_t column_i = -1);
  //TObject*                getTObjectPtr (TString branchname, Int_t row_i = -1, Int_t column_i = -1);
  //void*                   getVoidPtr (TString branchname, Int_t row_i = -1, Int_t column_i = -1);

  ClassDef(AnalysisTreeReader, 0);

private:
  // storage for basic types
  std::vector<Bool_t>                   fB;
  std::vector<Byte_t>                   fBy; // unsigned char
  std::vector<Int_t>                    fI;
  std::vector<UInt_t>                   fUI;
  std::vector<Short_t>                  fS;
  std::vector<UShort_t>                 fUS;
  std::vector<Long64_t>                 fL64;
  std::vector<long long int>            flli;
  std::vector<ULong64_t>                fUL64;
  std::vector<unsigned long long int>   fulli;
  std::vector<Float16_t>                fF16;
  std::vector<Double32_t>               fD32;
  std::vector<long double>              fld;
  std::vector<Char_t>                   fC;
  std::vector<TString>                  fTS;
  std::vector<TObject*>                 fTOptr;

  // storage for 1D arrays of data
  std::vector<std::vector<Bool_t> >                   fvB;
  std::vector<std::vector<Byte_t> >                   fvBy;
  std::vector<std::vector<Int_t> >                    fvI;
  std::vector<std::vector<UInt_t> >                   fvUI;
  std::vector<std::vector<Short_t> >                  fvS;
  std::vector<std::vector<UShort_t> >                 fvUS;
  std::vector<std::vector<Long64_t> >                 fvL64;
  std::vector<std::vector<long long int> >            fvlli;
  std::vector<std::vector<ULong64_t> >                fvUL64;
  std::vector<std::vector<unsigned long long int> >   fvulli;
  std::vector<std::vector<Float16_t> >                fvF16;
  std::vector<std::vector<Double32_t> >               fvD32;
  std::vector<std::vector<long double> >              fvld;
  std::vector<std::vector<Char_t> >                   fvC;
  std::vector<std::vector<TString> >                  fvTS;
  std::vector<std::vector<TObject*> >                 fvTOptr;

  // storage for 2D arrays of data
  std::vector<std::vector<std::vector<Bool_t> > >                   fvvB;
  std::vector<std::vector<std::vector<Byte_t> > >                   fvvBy;
  std::vector<std::vector<std::vector<Int_t> > >                    fvvI;
  std::vector<std::vector<std::vector<UInt_t> > >                   fvvUI;
  std::vector<std::vector<std::vector<Short_t> > >                  fvvS;
  std::vector<std::vector<std::vector<UShort_t> > >                 fvvUS;
  std::vector<std::vector<std::vector<Long64_t> > >                 fvvL64;
  std::vector<std::vector<std::vector<long long int> > >            fvvlli;
  std::vector<std::vector<std::vector<ULong64_t> > >                fvvUL64;
  std::vector<std::vector<std::vector<unsigned long long int> > >   fvvulli;
  std::vector<std::vector<std::vector<Float16_t> > >                fvvF16;
  std::vector<std::vector<std::vector<Double32_t> > >               fvvD32;
  std::vector<std::vector<std::vector<long double> > >              fvvld;
  std::vector<std::vector<std::vector<Char_t> > >                   fvvC;
  std::vector<std::vector<std::vector<TString> > >                  fvvTS;
  std::vector<std::vector<std::vector<TObject*> > >                 fvvTOptr;

  TTree *fChain;
  Long64_t fEntry;
};

} /* HAL */ 

#endif
