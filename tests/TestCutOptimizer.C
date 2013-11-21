TestCutOptimizer(Int_t n = 2)
{
  gSystem->Load("libCutOptimizer");
  gSystem->Load("libAnalysisUtils");

  std::cout << "\nTesting Optimize(sig, bkg, " << n << ")" << std::endl;
  TH1D *sig = HAL::getTH1FromFileName("aux/SigDRll240_processed.root", "DR ll");
  TH1D *bkg = HAL::getTH1FromFileName("aux/BkgDRll_processed.root", "DR ll");
  HAL::CutOptimizer co;
  TMatrixD *range = co.Optimize(sig, bkg, n);
  range->Print();
  delete sig;
  delete bkg;
  delete range;
}
