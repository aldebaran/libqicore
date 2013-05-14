%module qicore

%include "std_string.i"

%{
  #include <qicore-compat/api.hpp>
  #include <qicore-compat/box.hpp>
  #include <qicore-compat/statemachine.hpp>
  #include <qicore-compat/transition.hpp>

%}

%include <qicore-compat/api.hpp>
%include <qicore-compat/box.hpp>
%include <qicore-compat/statemachine.hpp>
%include <qicore-compat/transition.hpp>
