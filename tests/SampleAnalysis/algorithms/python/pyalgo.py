import HAL

#class PythonAlgo(HAL.PythonReconstructionAlgorithm):
class PythonAlgo(HAL.ReconstructionAlgorithm):

  """Docstring for PythonAlgo. """
  def Init(self, option = ''):
    print "python initialization"

  def Exec(self, option = ''):
    print "python executable"
 
  def Clear(self, option = ''):
    print "python clear"
