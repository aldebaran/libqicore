#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import sys
import logging

import allog

import qicore
import qimessagingswig


# This class just bind a Timeline and a StateMachine
class TimelineLegacy:
  def __init__(self, timeline, stateMachine):
    self._stateMachine = stateMachine
    self._timeline = timeline

  def goTo(self, label):
    frame = label
    if (self._stateMachine is not None):
      frame = self._stateMachine.goToLabel(label)
    if (self._timeline is not None):
      self._timeline.goTo(frame)

class BehaviorLogHandler(logging.Handler):
  def __init__(self):
    logging.Handler.__init__(self)

  def emit(self, record):
    level_to_function = {
      logging.DEBUG: allog.debug,
      logging.INFO: allog.info,
      logging.WARNING: allog.warning,
      logging.ERROR: allog.error,
      logging.CRITICAL: allog.fatal,
    }
    function = level_to_function.get(record.levelno, allog.debug)
    function(record.getMessage(),
             record.name,
             record.filename,
             record.funcName,
             record.lineno)

class BehaviorLegacy(qicore.Box):
  def __init__(self, name):
    qicore.Box.__init__(self)
    self.setName(name)
    self.resource = False
    self._loadCount = 0
    self._parentBox = None
    self.behaviorId = name

    # Register default callbacks
    self.registerOnLoadCallback(self.__onLoad__)
    self.registerOnUnloadCallback(self.__onUnload__)

    self._inputSignalsMap = {}
    self._outputSignalsMap = {}
    self._parameterMaps = {}
    self._callbackIdMap = {}
    self._connectionCounter = {}

    self.logger = logging.getLogger(name)
    logHandler = BehaviorLogHandler()
    self.logger.addHandler(logHandler)
    self.logger.setLevel(logging.DEBUG)

    self._previousOnLoad = []


  def printDebug(self, mystr):
    self.logger.debug(str(self.getName() + " : " + mystr))

  def printWarn(self, mystr):
    self.logger.warning(str(self.getName() + " : " + mystr))

  def printError(self, mystr):
    self.logger.error(str(self.getName() + " : " + mystr))

  def printInfo(self, mystr):
    self.logger.info(str(self.getName() + " : " + mystr))

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

  def _findUserMethod(self, methodName):
    if (methodName in dir(self)):
      return getattr(self, methodName)
    else:
      return None

  def addCallbackToSignal(self, signalName, callback):
    if (signalName in self._inputSignalsMap):
      return self._inputSignalsMap[signalName].connect(callback)
    elif (signalName in self._outputSignalsMap):
      return self._outputSignalsMap[signalName].connect(callback)
    else:
      return None

  def removeCallbackToSignal(self, signalName, callbackId):
    if (signalName in self._inputSignalsMap):
      return self._inputSignalsMap[signalName].connect(callbackId)
    elif (signalName in self._outputSignalsMap):
      return self._outputSignalsMap[signalName].connect(callbackId)
    else:
      return False

  def connectInput(self, inputName, target, signalName):
    self.printDebug("ConnectInput " + inputName + " from " + self.getName()
        + " to " + signalName + " from " + target.getName())

    methodName = "onInput_" + inputName + "__"
    connectionName = str(methodName + signalName)

    if (connectionName in self._connectionCounter):
      self.printDebug("Already connected: increment connection counter")
      self._connectionCounter[connectionName] = self._connectionCounter[connectionName] + 1
    else:
      f = self._findUserMethod(methodName)
      if (f is None):
        self.printWarn("Unable to find " + methodName)
      else:
        signalId = target.addCallbackToSignal(signalName, f)
        self._callbackIdMap[connectionName] = signalId
        self._connectionCounter[connectionName] = 1

  def connectOutput(self, outputName, target, signalName):
    self.printDebug("ConnectOutput " + outputName + " from " + self.getName()
        + " to " + signalName + " from " + target.getName())

    methodName = outputName
    connectionName = str(methodName + signalName)

    if (connectionName in self._connectionCounter):
      self.printDebug("Already connected: increment connection counter")
      self._connectionCounter[connectionName] = self._connectionCounter[connectionName] + 1
    else:
      f = self._findUserMethod(methodName)
      if (f is None):
        self.printWarn("Unable to find " + methodName)
      else:
        signalId = target.addCallbackToSignal(signalName, f)
        self._callbackIdMap[connectionName] = signalId
        self._connectionCounter[connectionName] = 1

  def connectParameter(self, parameterName, target, signalName):
    self.printDebug("ConnectParameter " + parameterName + " from " + self.getName()
        + " to " + signalName + " from " + target.getName())
    self.printError("Not implemented yet")
    sys.exit(2)

  def disconnectInput(self, inputName, target, signalName):
    self.printDebug("disconnectInput " + inputName + " from " + self.getName()
        + " to " + signalName + " from " + target.getName())

    methodName = "onInput_" + inputName + "__"
    connectionName = str(methodName + signalName)
    if (not (connectionName in self._connectionCounter)):
      self.printError("Unable to disconnect, this connection is unknown")
      return

    self._connectionCounter[connectionName] = self._connectionCounter[connectionName] - 1
    if (self._connectionCounter[connectionName] == 0):
      self.printDebug("Counter == zero : Unload")
      del self._connectionCounter[connectionName]
      target._inputSignalsMap[signalName].disconnect(self._callbackIdMap[connectionName])
      del self._callbackIdMap[connectionName]


  def disconnectOutput(self, outputName, target, signalName):
    self.printDebug("disconnectOutput " + outputName + " from " + self.getName()
        + " to " + signalName + " from " + target.getName())

    methodName = outputName
    connectionName = str(methodName + signalName)
    if (not (connectionName in self._connectionCounter)):
      self.printError("Unable to disconnect, this connection is unknown")
      return

    self._connectionCounter[connectionName] = self._connectionCounter[connectionName] - 1
    if (self._connectionCounter[connectionName] == 0):
      self.printDebug("Counter == zero : Unload")
      del self._connectionCounter[connectionName]
      target._outputSignalsMap[signalName].disconnect(self._callbackIdMap[connectionName])
      del self._callbackIdMap[connectionName]

  def disconnectParameter(self, parameterName, target, signalName):
    self.printDebug("disconnectParameter " + parameterName + " from " + self.getName()
        + " to " + signalName + " from " + target.getName())
    self.printError("Not implemented yet")
    sys.exit(2)

  def __onLoad__(self):
    # Load the box only once
    self.printDebug("Load with count = " + str(self._loadCount))

    self._loadCount = self._loadCount + 1;

    if (self._loadCount == 1):
      self._safeCallOfUserMethod("onLoad", None)


  def __onUnload__(self):
    self.printDebug("Unload with count = " + str(self._loadCount))

    if (self._loadCount > 0):
      self._loadCount = self._loadCount - 1;

    if (self._loadCount == 0):
      if (self.resource):
        self.releaseResource()
      self._safeCallOfUserMethod("onUnload", None)
      self.printDebug("Unload")

  # This method is called by the Timeline when the job is done
  def __onTimelineStopped__(self):
    if (self.hasStateMachine()):
      self.getStateMachine().stop()

    # Stimulate all outputs
    for name, sig in self._outputSignalsMap.items():
      sig.trigger(None)

  # This method is called by the state machine when entering a new state
  # This method will activate the right boxes
  def __onNewState__(self):
    self.stimulateIO("onLoad", None)
    self._previousOnLoad.append(self._inputSignalsMap["onLoad"])
    self._inputSignalsMap["onLoad"] = qimessagingswig.qi_signal()

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

  def log(self, string):
    self.printInfo(string)

  def setParentBox(self, box):
    self._parentBox = box


  # Compatibility layer for Parent Timeline Control
  def getParentTimeline(self):
    return TimelineLegacy(self._parentBox.getTimeline(), self._parentBox.getStateMachine())

  def stopTimelineParent(self):
    # Yup, Stop is just a pause in legacy format
    if (self._parentBox is None):
      return
    if (self._parentBox.hasTimeline()):
      self._parentBox.getTimeline().pause()
    if (self._parentBox.hasStateMachine()):
      self._parentBox.getStateMachine().pause()

  def playTimelineParent(self):
    if (self._parentBox is None):
      return
    if (self._parentBox.hasTimeline()):
      self._parentBox.getTimeline().play()
    if (self._parentBox.hasStateMachine()):
      self._parentBox.getStateMachine().run()

  # Compatibilty layer for FrameManager
  def getBehaviorPath(self, behaviorId):
    return self.getPath()

