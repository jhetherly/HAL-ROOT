import os.path, errno

# Helpful python functions
def moduleExists(module_name):
  try:
    __import__(module_name)
  except (KeyboardInterrupt, SystemExit):
    raise
  except:
    return False
  return True

def mkdirp(dir):
  try:                                                                                                                  
    os.makedirs(dir)                                                                                          
  except OSError as exc:                                                                                                
    if exc.errno == errno.EEXIST and os.path.isdir( working_dir ):                                                      
      pass                                                                                                              
    else:                                                                                                               
      raise

# Load in HAL C++
from ROOT import gSystem
gSystem.Load( 'libHAL.so' )
from ROOT import HAL

# Imported functions
from ROOT.HAL import getNextCombination     # not really needed since python has itertools
from ROOT.HAL import makeTLVFromPtEtaPhiE
from ROOT.HAL import makeTLVFromPtEtaPhiM
from ROOT.HAL import getTH1FromFileName
from ROOT.HAL import buildTH1D
from ROOT.HAL import buildTH1F
from ROOT.HAL import buildTH1I
from ROOT.HAL import buildTH1C
from ROOT.HAL import buildTH1S
from ROOT.HAL import buildTH2D
from ROOT.HAL import buildTH2F
from ROOT.HAL import buildTH2I
from ROOT.HAL import buildTH2C
from ROOT.HAL import buildTH2S
from ROOT.HAL import buildTH3D
from ROOT.HAL import buildTH3F
from ROOT.HAL import buildTH3I
from ROOT.HAL import buildTH3C
from ROOT.HAL import buildTH3S
from ROOT.HAL import standardLegendFormat
from ROOT.HAL import standardHistogramStackFormat

# Imported classes
from ROOT.HAL import CutOptimizer
from ROOT.HAL import Integrator 

