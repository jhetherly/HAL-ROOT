#include <HAL/Algorithms/Monitor.h>

namespace HAL
{

/*
 * Monitoring Algorithms
 * */

void Algorithms::MonitorAlgorithm::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  HAL::AnalysisTreeReader *tr = GetRawData();
  HAL::GenericData *input_data = NULL;

  if ((tr->GetEntryNumber() + 1) % fN == 0) {
    (*fOS) << "\nSummary for algorithm - " << fInput.Data() << std::endl;

    if (data->Exists(fInput))
      input_data = (GenericData*)data->GetTObject(fInput);
    else {
      (*fOS) << "Algorithm is empty!" << std::endl;
      return;
    }

    (*fOS) << *input_data << std::endl;
  }
}

void Algorithms::MonitorUserData::Exec (Option_t* /*option*/) {
  //HAL::AnalysisData *data = GetUserData();

  (*fOS) << "UserData has the following elements:" << std::endl;
}

} /* HAL */ 
