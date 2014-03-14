import HAL

class PythonAlgo(HAL.PythonReconstructionAlgorithm):

  """Docstring for PythonAlgo. """
  def Init(self, option = ''):
    print "python initialization"

  def Exec(self, option = ''):
    #self.GetData("RawData")
    print "python executable"
 
  def Clear(self, option = ''):
    print "python clear"
