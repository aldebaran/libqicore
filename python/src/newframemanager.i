%module newframemanager

%include "std_string.i"

%{
  #include <newframemanager/timeline.h>
  #include <newframemanager/box.h>
  #include <newframemanager/diagram.h>
  #include <newframemanager/state_machine.h>
  #include <newframemanager/state.h>
  #include <newframemanager/transition.h>
%}

%include <newframemanager/timeline.h>
%include <newframemanager/box.h>
%include <newframemanager/diagram.h>
%include <newframemanager/state_machine.h>
%include <newframemanager/state.h>
%include <newframemanager/transition.h>
