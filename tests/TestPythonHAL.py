import HAL
import time

def main():
  """Tests for the PyHAL module"""
  # Test cut optimization (Notice the terseness of both this and the 
  # C++ code in 'TestCutOptimizer.C')
  print "Starting test of CutOptimizer class."
  start_time = time.time()

  n = 5
  co = HAL.CutOptimizer()
  sig = HAL.getTH1FromFileName("aux/SigDRll240_processed.root", "DR ll");
  bkg = HAL.getTH1FromFileName("aux/BkgDRll_processed.root", "DR ll");
  range = co.Optimize(sig, bkg, n);

  # Print out the matrix of intervals (each row is an interval)
  range.Print();

  end_time = time.time()

  print "Time for {0}-sided cut optimization: {1}s".format( n, (end_time-start_time) )
  print "Finished test of CutOptimizer class."

if __name__ == "__main__":
  main()
