#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import sys

import qicore
import qimessagingswig

class behavior:
  def __init__(self, name):
    self._name = name
    self.resource = False
    self._loadCount = 0

    self._inputSignalsMap = {}
    self._outputSignalsMap = {}
    self._parameterMaps = {}


  def printDebug(self, mystr):
    print("[DEBUG] " + self.getName() + " : " + mystr)

  def printWarn(self, mystr):
    print("[WARN ] " + self.getName() + " : " + mystr)

  def printError(self, mystr):
    print("[ERROR] " + self.getName() + " : " + mystr)

  def printFatal(self, mystr):
    sys.stderr.write("[FATAL] " + self.getName() + " : " + mystr)

  def waitResourcesCallback(self, callbackName):
    self.printDebug("waitResourcesCallback " + callbackName)
    self.printError("Not implemented yet")

  def isResourceFree(self, resourceNameList):
    self.printDebug("isResourceFree")
    self.printError("Not implemented yet")
    return True

  def waitResourceFree(self):
    self.printDebug("waitResourceFree")
    self.printError("Not implemented yet")

  def waitResources(self):
    self.printDebug("waitResources")
    self.printError("Not implemented yet")

  def releaseResource(self):
    self.printDebug("releaseResource")
    self.printError("Not implemented yet")

  def addInput(self, inputName):
    self.printDebug("addInput with name " + inputName)
    self._inputSignalsMap[inputName] = qimessagingswig.qi_signal()

  def addOutput(self, outputName, isBang):
    self.printDebug("addOutput with name " + outputName + " isBang : " + str(isBang))
    self._outputSignalsMap[outputName] = qimessagingswig.qi_signal()
    self.printWarn("IsBang parameter is not used for the moment")

  def addParameter(self, parameterName, value, parentValue):
    self.printDebug("addParameter with name " + parameterName)
    self._parameterMaps[parameterName] = value
    self.printWarn("parentValue parameter is not used for the moment")

  def getParameter(self, parameterName):
    self.printDebug("getParameter with name " + parameterName)
    if (parameterName in self._parameterMaps):
      return self._parameterMaps[parameterName]
    else:
      self.printError("Parameter with name : " + parameterName + " is unknown")
      return None

  def getParametersList(self):
    self.printDebug("getParametersList")
    self.printError("Not implemented yet")
    sys.exit(2)

  def setParameter(self, parameterName, value):
    self.printDebug("setParameter with name " + parameterName)
    if (parameterName in self._parameterMaps):
      self._parameterMaps[parameterName] = value
    else:
      self.printError("Parameter with name : " + parameterName + " is unknown")

  def stimulateIO(self, name, param):
    self.printDebug("stimulateIO " + name)
    if (name in self._inputSignalsMap):
      self._inputSignalsMap[name].trigger(param)
    elif (name in self._outputSignalsMap):
      self._outputSignalsMap[name].trigger(param)
    else:
      self.printWarn("Unable to stimulate IO " + name + " : signal not found")

  def BIND_PYTHON(self, moduleName, methodName, paramCount):
    self.printDebug("BIND_PYTHON " + methodName + " from " + moduleName
          + " with " + str(paramCount))
    self.printWarn("This function is no more used")

  def getName(self):
    return self._name

  def addCallbackToSignal(self, signalName, callback):
    if (signalName in self._inputSignalsMap):
      self._inputSignalsMap[signalName].connect(callback)
    elif (signalName in self._outputSignalsMap):
      self._outputSignalsMap[signalName].connect(callback)
    else:
      self.printWarn("Unable to connect " + signalName + " : signal not found")

  def _findUserMethod(self, methodName):
    if (methodName in dir(self)):
      return getattr(self, methodName)
    else:
      return None

  def connectInput(self, inputName, target, signalName):
    self.printDebug("ConnectInput " + inputName + " from " + self.getName()
        + " to " + signalName + " from " + target.getName())
    methodName = "onInput_" + inputName + "__"
    f = self._findUserMethod(methodName)
    if (f is None):
      self.printWarn("Unable to find " + methodName)
    else:
      target.addCallbackToSignal(signalName, f)

  def connectOutput(self, outputName, target, signalName):
    self.printDebug("ConnectOutput " + outputName + " from " + self.getName()
        + " to " + signalName + " from " + target.getName())
    methodName = outputName
    f = self._findUserMethod(methodName)
    if (f is None):
      self.printWarn("Unable to find " + methodName)
    else:
      target.addCallbackToSignal(signalName, f)

  def connectParameter(self, parameterName, target, signalName):
    self.printDebug("ConnectParameter " + parameterName + " from " + self.getName()
        + " to " + signalName + " from " + target.getName())
    self.printError("Not implemented yet")
    sys.exit(2)

  def __onLoad__(self):
    # Load the box only once
    if (self._loadCount == 0):
      self._safeCallOfUserMethod("onLoad", None)
      self.stimulateIO("onLoad", None)

    self._loadCount = self._loadCount + 1;

  def __onUnload__(self):
    if (self._loadCount > 0):
      self._loadCount = self._loadCount - 1;

    if (self._loadCount == 0):
      if (self.resource):
        self.releaseResource()
      self._safeCallOfUserMethod("onUnload", None)

  def _safeCallOfUserMethod(self, functionName, functionArg):
    try:
      if(functionName in dir(self)):
        func = getattr(self, functionName)
        if(func.im_func.func_code.co_argcount == 2):
          func(functionArg)
        else:
          func()
      return True
    except BaseException, err:
      self.printFatal("Call to : " + functionName + " failed with : " + str(err))
      try:
        if("onError" in dir(self)):
          self.onError(self.getName() + ':' +str(err))
      except BaseException, err2:
        self.printError(str(err2))
    sys.exit(2)
    return False


class BehaviorLegacy(qicore.Box, behavior):
  def __init__(self, name):
    qicore.Box.__init__(self)
    self.setName(name)
    behavior.__init__(self, name)

    # Register default callbacks
    self.registerOnLoadCallback(self.__onLoad__)
    self.registerOnUnloadCallback(self.__onUnload__)

