from ROOT import gSystem

gSystem.Load( 'libAlgorithms.so' )
gSystem.Load( 'libAnalysisUtils.so' )
gSystem.Load( 'libCutOptimizer.so' )
gSystem.Load( 'libPlotUtils.so' )
gSystem.Load( 'libIntegrator.so' )

from ROOT import HAL
# functions
# libAlgorithms.so
from ROOT.HAL import getNextCombination     # not really needed since python has itertools
# libAnalysisUtils.so
from ROOT.HAL import makeTLVFromPtEtaPhiE
from ROOT.HAL import makeTLVFromPtEtaPhiM
from ROOT.HAL import getTH1FromFileName
# libPlotUtils.so
#from ROOT.HAL import buildTH1
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

# Classes
# libCutOptimizer.so
from ROOT.HAL import CutOptimizer
# libIntegrator.so
from ROOT.HAL import Integrator 

