%module qicore

%include "std_string.i"

%{
  #include <qicore/api.hpp>
  #include <qicore/timeline.hpp>
  #include <qicore/box.hpp>
  #include <qicore/statemachine.hpp>
  #include <qicore/state.hpp>
  #include <qicore/transition.hpp>

%}

%include <qicore/api.hpp>
%include <qicore/timeline.hpp>
%include <qicore/box.hpp>
%include <qicore/statemachine.hpp>
%include <qicore/state.hpp>
%include <qicore/transition.hpp>
