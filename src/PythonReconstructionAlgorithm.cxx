#include <HAL/PythonReconstructionAlgorithm.h>

//ClassImp(HAL::PythonReconstructionAlgorithm);

namespace HAL {

PythonReconstructionAlgorithm::PythonReconstructionAlgorithm (TString name, TString title, TString pyfile, TString pyclass, PyObject *self) : 
  ReconstructionAlgorithm(name, title), fPyFile(pyfile), fPyClass(pyclass) {
  if ( self ) {
    // steal reference as this is us, as seen from python
    fPySelf = self;
  } else {
    Py_INCREF( Py_None );        // using None allows clearer diagnostics
    fPySelf = Py_None;
  }
}

PythonReconstructionAlgorithm::~PythonReconstructionAlgorithm () {
  // Destructor. Only deref if still holding on to Py_None (circular otherwise).
  if ( fPySelf == Py_None ) {
    Py_DECREF( fPySelf );
  }
}

void  PythonReconstructionAlgorithm::Init (Option_t *options) {
  // First function called, and used to setup the python self; forward call.
  SetupPySelf();

  // As per the generated code: the tree argument is deprecated (on PROOF 0 is
  // passed), and hence not forwarded.
  PyObject* result = CallSelf( "Init" );

  if ( ! result )
    Abort();

  Py_XDECREF( result );
}

void  PythonReconstructionAlgorithm::Exec (Option_t *options) {
  // As per the generated code: the tree argument is deprecated (on PROOF 0 is
  // passed), and hence not forwarded.
  PyObject* result = CallSelf( "Exec" );

  if ( ! result )
    Abort();

  Py_XDECREF( result );
}

void  PythonReconstructionAlgorithm::Clear (Option_t *options) {
  // As per the generated code: the tree argument is deprecated (on PROOF 0 is
  // passed), and hence not forwarded.
  PyObject* result = CallSelf( "Clear" );

  if ( ! result )
    Abort();

  Py_XDECREF( result );
}

void PythonReconstructionAlgorithm::SetupPySelf() {
  // Install the python side identity of the PythonReconstructionAlgorithm
  if ( fPySelf && fPySelf != Py_None )
    return;                      // already created ...

  //// split option as needed for the module part and the (optional) user part
  //std::string opt = GetOption();
  //std::string::size_type pos = opt.find( '#' );
  //std::string module = opt.substr( 0, pos );
  //std::string user = (pos == std::string::npos) ? "" : opt.substr( pos+1, std::string::npos );

  //TString impst = TString::Format( "import %s", module.c_str() );

  //// reset user option
  //SetOption( user.c_str() );

  //// use TPython to ensure that the interpreter is initialized
  //if ( ! TPython::Exec( (const char*)impst ) ) {
  //  Abort( "failed to load provided python module" );  // Exec already printed error trace
  //  return;
  //}

  // get the PythonReconstructionAlgorithm python class
  PyObject* tpysel = PyObject_GetAttrString(
      PyImport_AddModule( const_cast< char* >( "libHAL.so" ) ),
      const_cast< char* >( "PythonReconstructionAlgorithm" ) );

  // get handle to the module
  PyObject* pymod = PyImport_AddModule( const_cast< char* >( fPyFile.Data() ) );

  // get the module dictionary to loop over
  PyObject* dict = PyModule_GetDict( pymod );
  Py_INCREF( dict );

  // locate the TSelector derived class
  PyObject* allvalues = PyDict_Values( dict );

  PyObject* pyclass = 0;
  for ( int i = 0; i < PyList_GET_SIZE( allvalues ); ++i ) {
    PyObject* value = PyList_GET_ITEM( allvalues, i );
    Py_INCREF( value );

    if ( PyType_Check( value ) && PyObject_IsSubclass( value, tpysel ) ) {
      if ( PyObject_RichCompareBool( value, tpysel, Py_NE ) ) {   // i.e., if not equal
        pyclass = value;
        break;
      }
    }

    Py_DECREF( value );
  }

  Py_DECREF( allvalues );
  Py_DECREF( dict );
  Py_DECREF( tpysel );

  if ( ! pyclass ) {
    Abort();
    return;
  }

  PyObject* args = PyTuple_New( 0 );
  PyObject* self = PyObject_Call( pyclass, args, 0 );
  Py_DECREF( args );
  Py_DECREF( pyclass );

  // final check before declaring success ...
  if ( !self ) {
    if ( !PyErr_Occurred() )
      PyErr_SetString( PyExc_RuntimeError, "could not create python reco algo" );
    Py_XDECREF( self );
    Abort();
    return;
  }

  // steal reference to new self, since the deletion will come from the C++ side
  Py_XDECREF( fPySelf );
  fPySelf = self;

  //// inject ourselves into the base of self; destroy old identity if need be (which happens
  //// if the user calls the default ctor unnecessarily)
  //TPySelector* oldselector = (TPySelector*)((PyROOT::ObjectProxy*)fPySelf)->fObject;
  //((PyROOT::ObjectProxy*)fPySelf)->fObject = this;
  //if ( oldselector ) {
  //  PyROOT::TMemoryRegulator::UnregisterObject( oldselector );
  //  delete oldselector;
  //}
}

PyObject* PythonReconstructionAlgorithm::CallSelf(const char *method, PyObject *pyobject) {
  // Forward <method> to python.
  if ( !fPySelf || fPySelf == Py_None ) {
    Py_INCREF( Py_None );
    return Py_None;
  }

  PyObject *result = 0;

  // get the named method and check for python side overload by not accepting the
  // binding's methodproxy
  PyObject* pymethod = PyObject_GetAttrString( fPySelf, const_cast< char* >( method ) );
  //if ( ! PyROOT::MethodProxy_CheckExact( pymethod ) ) {
    if ( pyobject )
      result = PyObject_CallFunction( pymethod, const_cast< char* >( "O" ), pyobject );
    else
      result = PyObject_CallFunction( pymethod, const_cast< char* >( "" ) );
  //} else {
  //  // silently ignore if method not overridden (note that the above can't lead
  //  // to a python exception, since this (TPySelector) class contains the method
  //  // so it is always to be found)
  //  Py_INCREF( Py_None );
  //  result = Py_None;
  //}

  Py_XDECREF( pymethod );

  if ( !result )
    Abort();

  return result;
}

} /* HAL */ 
