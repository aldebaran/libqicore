#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import os
import sys

class IOType:
  DYNAMIC=0
  BANG=1
  NUMBER=2
  STRING=3
  BITMAP=4
  SOUND=5

class InputType:
  ONLOAD=0
  UNDEF=1
  ONSTART=2
  ONSTOP=3
  STMVALUE=4

class OutputType:
  UNDEF=0
  STOPPED=1
  PUNCTUAL=2
  RECURRENT=3

class ParameterType:
  BOOL=0
  INT=1
  DOUBLE=2
  STRING=3
  RESOURCE=4

class ResourceMode:
  LOCK="Lock"
  STOP_ON_DEMAND="Stop on demand"
  PAUSE_ON_DEMAND="Pause on demand"
  CALLBACK_ON_DEMAND="Callback on demand"

class patcher:
  def __init__(self, box):
    self._box = box
    self._code = box.script.content.lstrip()
    self._addedMethods = ""
    self._indentForInit = 4
    self._indentForMethod = 4
    self._inputMethodMap = { InputType.ONLOAD : patcher.addInputMethod_onLoad,
                             InputType.UNDEF : patcher.addInputMethod_unDef,
                             InputType.ONSTART : patcher.addInputMethod_onStart,
                             InputType.ONSTOP : patcher.addInputMethod_onStop,
                             InputType.STMVALUE: patcher.addInputMethod_STMValue}
    self._outputMethodMap = { OutputType.STOPPED : patcher.addOutputMethod_Stopped,
                              OutputType.PUNCTUAL : patcher.addOutputMethod_Punctual}
    self._paramMethodMap = {}
    self._resourceMethodMap = {ResourceMode.LOCK : patcher.addResourceMethod_Lock,
                               ResourceMode.STOP_ON_DEMAND: patcher.addResourceMethod_Stop_on_demand,
                               ResourceMode.PAUSE_ON_DEMAND: patcher.addResourceMethod_Pause_on_demand,
                               ResourceMode.CALLBACK_ON_DEMAND: patcher.addResourceMethod_Callback_on_demand}

  def findInitIndentation(self):
    splittedStr = self._code.split(os.linesep)
    for s in splittedStr:
      if (("GeneratedClass.__init__(self)" in s)
          or ("GeneratedClass.__init__(self, False)" in s)):
        self._indentForInit = len(s) - len(s.lstrip())

  def findMethodIndentation(self):
    splittedStr = self._code.split(os.linesep)
    for s in splittedStr:
      if ("def __init__(" in s):
        self._indentForMethod = len(s) - len(s.lstrip())

  def formatParameter(self, name, content_type):
    if (content_type == ParameterType.STRING or content_type == ParameterType.RESOURCE):
      return "\"" + name + "\""
    else:
      return name

  def constructInitCode(self):
    indent = self._indentForInit
    initCode = "qicoreLegacy.BehaviorLegacy.__init__(self, \"" + self._box.name + "\", True)" + os.linesep
    initCode += indent * " " + "self.boxName = \"" + self._box.name + "\"" + os.linesep
    initCode += indent * " " + "self.setName(\"" + self._box.name + "\")" + os.linesep
    initCode += indent * " " + "self.setBroker(broker.getALBroker())" + os.linesep

    for inp in self._box.inputs:
      if (self.addInputMethod(inp.name, int(inp.nature))):
        initCode += (indent * " " + "self.BIND_PYTHON(self.getName(), \"onInput_" + inp.name + "__\", 1)" +
            os.linesep)
        initCode += (indent * " " + "self.addInput(\"" + inp.name + "\")" + os.linesep)

    for out in self._box.outputs:
      if (self.addOutputMethod(out.name, int(out.nature))):
        initCode += (indent * " " + "self.BIND_PYTHON(self.getName(), \"onOutput_" + out.name + "__\", 1)" +
                      os.linesep)
        initCode +=  (indent * " "
                      + "self.addOutput(\""
                      # FIXME: True or false if bang
                      + out.name + "\", True)" + os.linesep)

    for param in self._box.parameters:
      initCode += (indent * " "
                   + "self.addParameter(\"" + param.name
                  # FIXME: True or False random ? xD
                   + "\", " + self.formatParameter(param.value, int(param.content_type)) + ", True)"
                   + os.linesep)

    if (len(self._box.resources) != 0):
      for res in self._box.resources:
        self.addResourceMethod(res.type)

    return initCode.rstrip()

  def addInheritance(self):
    self._code = self._code.replace("(GeneratedClass):", "(qicoreLegacy.BehaviorLegacy):", 1)

  def addInputMethod_onLoad(self, inpName):
    indent = self._indentForMethod
    self._addedMethods += (indent * " " + "def onInput_" + inpName + "__(self, p):" + os.linesep
                           + indent * " " * 2
                           + "self.__onLoad__()" + os.linesep
                           + indent * " " * 2 + "self.stimulateIO(\"" + inpName + "\", p)"
                           + os.linesep * 2)
    return True

  def addInputMethod_STMValue(self, inpName):
    return False

  def addInputMethod_onStart(self, inpName):
    indent = self._indentForMethod
    self._addedMethods += (indent * " " + "def onInput_" + inpName + "__(self, p):" + os.linesep
                           + indent * " " * 2
                           + "if(not self._safeCallOfUserMethod(\"onInput_" + inpName + "\", p)):" + os.linesep
                           + indent * " " * 3 + "self.releaseResource()" + os.linesep
                           + indent * " " * 2 + "if (self.hasTimeline()):" + os.linesep
                           + indent * " " * 3 + " self.getTimeline().play()" + os.linesep
                           + indent * " " * 2 + "if (self.hasStateMachine()):" + os.linesep
                           + indent * " " * 3 + " self.getStateMachine().run()" + os.linesep
                           + indent * " " * 2 + "self.stimulateIO(\"" + inpName + "\", p)" + os.linesep
                           + os.linesep * 2)
    return True

  def addInputMethod_onStop(self, inpName):
    indent = self._indentForMethod
    self._addedMethods += (indent * " " + "def onInput_" + inpName + "__(self, p):" + os.linesep
                           + indent * " " * 2
                           + "if(not self._safeCallOfUserMethod(\"onInput_" + inpName + "\", p)):" + os.linesep
                           + indent * " " * 3 + "self.releaseResource()" + os.linesep
                           + indent * " " * 3 + "return" + os.linesep
                           + indent * " " * 2 + "self.stimulateIO(\"" + inpName + "\", p)" + os.linesep
                           + os.linesep * 2)
    return True

  def addInputMethod_unDef(self, inpName):
    indent = self._indentForMethod
    self._addedMethods += (indent * " " + "def onInput_" + inpName + "__(self, p):" + os.linesep
                           + indent * " " * 2
                           + "if(not self._safeCallOfUserMethod(\"onInput_" + inpName + "\", p)):" + os.linesep
                           + indent * " " * 3 + "self.releaseResource()" + os.linesep
                           + indent * " " * 3 + "return" + os.linesep
                           + indent * " " * 2 + "self.stimulateIO(\"" + inpName + "\", p)" + os.linesep
                           + os.linesep * 2)
    return True

  def addInputMethod(self, inpName, inpType):
    if (inpType in self._inputMethodMap):
      return self._inputMethodMap[inpType](self, inpName)
    else:
      print("Input Type not supported yet: ", inpType)
      sys.exit(2)

  def addOutputMethod_Stopped(self, outName):
    indent = self._indentForMethod
    self._addedMethods += (indent * " " + "def " + outName + "(self, p = None):" + os.linesep
                           + indent * " " * 2 + "if (self.hasTimeline()):" + os.linesep
                           + indent * " " * 3 + " self.getTimeline().stop()" + os.linesep
                           + indent * " " * 2 + "if (self.hasStateMachine()):" + os.linesep
                           + indent * " " * 3 + " self.getStateMachine().stop()" + os.linesep
                           + indent * " " * 2 + "self.stimulateIO(\"" + outName + "\", p)" + os.linesep * 2)
    return True

  def addOutputMethod_Punctual(self, outName):
    indent = self._indentForMethod
    self._addedMethods += (indent * " " + "def " + outName + "(self, p = None):" + os.linesep
                           + indent * " " * 2 + "self.stimulateIO(\"" + outName + "\", p)" + os.linesep * 2)
    return True

  def addOutputMethod(self, outName, outType):
    if (outType in self._outputMethodMap):
      return self._outputMethodMap[outType](self, outName)
    else:
      print("Output Type not supported yet: ", outType)
      sys.exit(2)

  def addResourceMethod(self, resType):
    if (resType in self._resourceMethodMap):
      return self._resourceMethodMap[resType](self)
    else:
      print("Resource Mode not supported yet: ", resType)
      sys.exit(2)

  def addResourceMethod_Lock(self):
    indent = self._indentForMethod
    self._addedMethods += (indent * " " + "def __OnResource__(self, resourceName):" + os.linesep
                           + indent * " " * 2 + "pass" + os.linesep * 2)

  def addResourceMethod_Stop_on_demand(self):
    indent = self._indentForMethod
    self._addedMethods += (indent * " " + "def __OnResource__(self, resourceName):" + os.linesep
                            + indent * " " * 2 + "bExist = True" + os.linesep
                            + indent * " " * 2 + "try:" + os.linesep
                            + indent * " " * 3 + "self.onResourceLost()" + os.linesep
                            + indent * " " * 2 + "except:" + os.linesep
                            + indent * " " * 3 + "try:" + os.linesep
                            + indent * " " * 4 + "self.onResourceLost(None)" + os.linesep
                            + indent * " " * 3 + "except:" + os.linesep
                            + indent * " " * 4 + "bExist = False" + os.linesep
                            + indent * " " * 2 + "if (self.hasTimeline()):" + os.linesep
                            + indent * " " * 3 + "self.getTimeline().stop()" + os.linesep
                            + indent * " " * 2 + "if (self.StateMachine()):" + os.linesep
                            + indent * " " * 3 + "self.getStateMachine().stop()" + os.linesep
                            + indent * " " * 2 + "self.releaseResource()" + os.linesep
                            + indent * " " * 2 + "if (not bExist):" + os.linesep
                            + indent * " " * 3 + "try:" + os.linesep
                            + indent * " " * 4 + "self.onStopped()" + os.linesep
                            + indent * " " * 3 + "except:" + os.linesep
                            + indent * " " * 4 + "try:" + os.linesep
                            + indent * " " * 5 + "self.onStopped(None)" + os.linesep
                            + indent * " " * 4 + "except:" + os.linesep
                            + indent * " " * 5 + "pass" + os.linesep)

  def addResourceMethod_Pause_on_demand(self):
    indent = self._indentForMethod
    self._addedMethods += (indent * " " + "def __OnResource__(self, resourceName):" + os.linesep
                          + indent * " " * 2 + "if (self.hasTimeline()):" + os.linesep
                          + indent * " " * 3 + "self.getTimeline().pause()" + os.linesep
                          + indent * " " * 2 + "if (self.hasStateMachine()):" + os.linesep
                          + indent * " " * 3 + "self.getStateMachine().pause()" + os.linesep
                          + indent * " " * 2 + "self.releaseResource()" + os.linesep
                          + indent * " " * 2 + "self.waitResourceFree()" + os.linesep
                          + indent * " " * 2 + "self.waitResources()" + os.linesep
                          + indent * " " * 2 + "if (self.hasTimeline()):" + os.linesep
                          + indent * " " * 3 + "self.getTimeline().play()" + os.linesep
                          + indent * " " * 2 + "if (self.hasStateMachine()):" + os.linesep
                          + indent * " " * 3 + "self.getStateMachine().play()" + os.linesep * 2)

  def addResourceMethod_Callback_on_demand(self):
    indent = self._indentForMethod
    self._addedMethods += (indent * " " + "def __OnResource__(self, resourceName):" + os.linesep
                            + indent * " " * 2 +"self._safeCallOfUserMethod(\"onResource\", resourceName)" + os.linesep * 2)

  def generateClass(self):
    self._code += ("class " + self._box.name + "_class" + "(GeneratedClass):" + os.linesep
                    + "  def __init__(self):" + os.linesep
                    + "    GeneratedClass.__init__(self)" + os.linesep + os.linesep)

  def replace_not_support_code(self):
    # Code that require a translation ...
    warning = "# /!!\ This code has been removed by the qicore Converter, no more supported..."
    self._code = self._code.replace("ALFrameManager", "self")
    #FIXME: This value cannot be acquired at this time...
    self._code = self._code.replace("self.getTimelineFps(self.getName())", "25")

    # SetFPS has only a meaning when Timeline has an actuator curve
    if (self._box.child and len(self._box.child.actuatorList) != 0):
      self._code = self._code.replace("self.setTimelineFps(self.getName(), newfps)", "self.getTimeline().setFPS(newfps)")
    else:
      self._code = self._code.replace("self.setTimelineFps(self.getName(), newfps)", "pass")
    self._code = self._code.replace("self.getBehaviorPath(self.behaviorId)", "self.getPath()")

  def patch(self):
    if (self._code.lstrip() == ""):
      self.generateClass()
    self._code = self._code.replace("MyClass", self._box.name + "_class", 1)
    # Replace tabs to normalize code
    self._code = self._code.replace("\t", "  ")
    self._code = ( "#!/usr/bin/env python" + os.linesep
                  + "# -*- coding: utf-8 -*-" + os.linesep * 2
                  + "import naoqi" + os.linesep
                  + "from naoqi import *" + os.linesep
                  + "import qicore" + os.linesep
                  + "import qicoreLegacy" + os.linesep * 2
                  + self._code)
    self._code = self._code.replace("__init__(self)", "__init__(self, broker)", 1)
    self.findInitIndentation()
    self.findMethodIndentation()
    self.addInheritance()
    initCode = self.constructInitCode()
    if ("GeneratedClass.__init__(self)" in self._code):
      self._code = self._code.replace("GeneratedClass.__init__(self)", initCode)
    else:
      self._code = self._code.replace("GeneratedClass.__init__(self, False)", initCode)
    self.replace_not_support_code()
    self._code += self._addedMethods
    return self._code

