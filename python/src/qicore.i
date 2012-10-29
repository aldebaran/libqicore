%module qicore

%include "std_string.i"

%{
  #include <qicore/timeline.h>
  #include <qicore/box.h>
  #include <qicore/diagram.h>
  #include <qicore/state_machine.h>
  #include <qicore/state.h>
  #include <qicore/transition.h>
%}

%include <qicore/timeline.h>
%include <qicore/box.h>
%include <qicore/diagram.h>
%include <qicore/state_machine.h>
%include <qicore/state.h>
%include <qicore/transition.h>
