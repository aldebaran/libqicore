/* File : example.i */
%module newinaoqi

//those overload avoid the generic function (ALValue) to be used and recognised as a catch all.
%ignore AL::ALMemoryProxy::insertData(std::string const &,int const &);
%ignore AL::ALMemoryProxy::insertData(std::string const &,float const &);
%ignore AL::ALMemoryProxy::insertData(std::string const &,std::string const &);

%{
#include <newframemanager/newinaoqi.h>
#include <alproxies/almemoryproxy.h>
#include <alpythontools/alpythontools.h>
using namespace AL;
%}

 %include stl.i
 %include exception.i

namespace std {
    %template(StringVector)    vector<std::string>;
}

 %exception {
        try {
        $function
        }
        catch (const AL::ALError& e) {
                std::cout << "==== Error Swig ====" << std::endl;
                std::cout << e.toString().c_str() << std::endl;
                SWIG_exception(SWIG_RuntimeError, e.toString().c_str());
                return NULL;
        }
}


%typemap(in) AL::ALValue const & {
   $1 = new ALValue(AL::ALPythonTools::convertPyObjectToALValue($input));

}

%typemap(freearg)  AL::ALValue const & {
   if ($1) delete($1);
}

%typemap(out) AL::ALValue
{
        $result = AL::ALPythonTools::convertALValueToPython($1);
}


%typemap(out) ALValue
{
              $result = AL::ALPythonTools::convertALValueToPython($1);
}




%typemap(out) std::string {
        $result = PyString_FromStringAndSize($1.c_str(), $1.size());
}


%typemap(in) (PyList)
{
        printf("LIST DETECTED -> todo !");
        $result = "test";
}

%typemap(in) char ** {
        printf("ARRAY DETECTED -> todo !");
        $result = "test";
        }

%inline %{
  extern void  setInstance(long a);
  #ifdef getMemoryProxy
  #undef getMemoryProxy
  #endif
  extern AL::ALMemoryProxy getMemoryProxy();
  extern bool _ALSystem(const char *pCommand);
%}

/* Let's just grab the original header file here */
%include "../newframemanager/newinaoqi.h"
%include "std_string.i"
#define ALPROXIES_API
%include <alproxies/almemoryproxy.h>
