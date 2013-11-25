import HAL
import ROOT
import time

def main():
  """Tests for the PyHAL module"""
  # Test cut optimization (Notice the terseness of both this and the 
  # C++ code in 'TestCutOptimizer.C')
  print "Starting test of CutOptimizer class."
  start_time = time.time()

  n = 2
  co = HAL.CutOptimizer()
  sig = HAL.getTH1FromFileName("aux/SigDRll240_processed.root", "DR ll");
  bkg = HAL.getTH1FromFileName("aux/BkgDRll_processed.root", "DR ll");
  range = co.Optimize(sig, bkg, n);

  # Print out the matrix of intervals (each row is an interval)
  range.Print();

  end_time = time.time()

  print "Time for {0}-sided cut optimization: {1}s".format( n, (end_time-start_time) )
  print "Finished test of CutOptimizer class.\n"

  # Test Integrator
  print "Starting test of Integrator class."
  start_time = time.time()
  f1 = ROOT.TF1("", "1.0/sqrt(x)", 0, 1)
  f2 = ROOT.TF1("", "pow(x, 2)*sin(1.0/x)", -1, 2)
  f3 = ROOT.TF1("", "exp(-1.0*pow(x-1.0, 2))", -1000, 1000)
  f4 = ROOT.TF1("", "cos(sin(x)/pow(x, 1.2))/pow(x, .3)", 0, 1)
  integrator = HAL.Integrator()
  f1_root = f1.Integral(0, 1)
  f1_hal = integrator.Integrate(f1, 0, 1)
  f2_root = f2.Integral(-1, 2)
  f2_hal = integrator.Integrate(f2, -1, 2)
  f3_root = f3.Integral(-1000, 1000)
  f3_hal = integrator.Integrate(f3, -1000, 1000)
  f4_root = f4.Integral(0, 1)
  f4_hal = integrator.Integrate(f4, 0, 1)
  end_time = time.time()

  print "\n{0}\t\t\t\t\t{1}\t\t{2}\t\t{3}".format("Integral", "ROOT", "HAL", "Actual Value")
  print "{0}\t\t{1}\t{2}\t{3}".format("pow(x, 2)*sin(1.0/x) from -1 to 2:", f2_root, f2_hal, 1.38755)
  print "{0}\t{1}\t\t{2}\t{3}".format("exp(-1.0*pow(x-1.0, 2)) from -1000 to 1000:", f3_root, f3_hal, 1.77245)
  print "{0}\t{1}\t{2}\t{3}".format("cos(sin(x)/pow(x, 1.2))/pow(x, .3) from 0 to 1:", f4_root, f4_hal, 0.358465)
  print "{0}\t\t\t\t{1}\t{2}\t\t{3}".format("1/sqrt(x) from 0 to 1:", f1_root, f1_hal, 2)
  print "\nTime for all integrations: {0}s".format( (end_time-start_time) )
  print "Finished test of Integrator class.\n"

if __name__ == "__main__":
  main()
