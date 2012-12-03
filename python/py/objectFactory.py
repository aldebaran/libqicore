#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import sys
import os
import io
import time
import types
import xml.dom.minidom

import qicore
import qicoreLegacy
from converter.xar_types import *

# Function generator for Input, Outputs, ...

def generateInputMethod(methodType, name):
  def onStart(self, p):
    if(not self._safeCallOfUserMethod("onInput_" + name, p)):
      self.releaseResource()
    if (self.hasTimeline()):
      self.getTimeline().play()
    if (self.hasStateMachine()):
      self.getStateMachine().run()
      self.stimulateIO(name, p)

  def onStop(self, p):
    if(not self._safeCallOfUserMethod("onInput_" + name, p)):
      self.releaseResource()
      return
    self.stimulateIO(name, p)

  inputMethodMap = { InputType.ONLOAD : None,
                     InputType.UNDEF : onStop,
                     InputType.ONSTART : onStart,
                     InputType.ONSTOP : onStop,
                     InputType.STMVALUE: None}

  return inputMethodMap[methodType]

def generateOutputMethod(methodType, name):
  def stopped(self, p = None):
    if (self.hasTimeline()):
      self.getTimeline().stop()
    if (self.hasStateMachine()):
      self.getStateMachine().stop()
    self.stimulateIO(name, p)

  def punctual(self, p = None):
    self.stimulateIO(name, p)

  outputMethodMap = { OutputType.STOPPED : stopped,
                      OutputType.PUNCTUAL : punctual}

  return outputMethodMap[methodType]

def generateResourceMethod(resourceType):
  def lock(self, resourceName):
    pass

  def stop_on_demand(self, resourceName):
    bExist = True
    try:
      self.onResourceLost()
    except:
      try:
        self.onResourceLost(None)
      except:
        bExist = False
    if (self.hasTimeline()):
      self.getTimeline().stop()
    if (self.StateMachine()):
      self.getStateMachine().stop()
    self.releaseResource()
    if (not bExist):
      try:
        self.onStopped()
      except:
        try:
          self.onStopped(None)
        except:
          pass

  def pause_on_demand(self, resourceName):
    if (self.hasTimeline()):
      self.getTimeline().pause()
    if (self.hasStateMachine()):
      self.getStateMachine().pause()
    self.releaseResource()
    self.waitResourceFree()
    self.waitResources()
    if (self.hasTimeline()):
      self.getTimeline().play()
    if (self.hasStateMachine()):
      self.getStateMachine().play()

  def callback_on_demand(self, resourceName):
    self._safeCallOfUserMethod("onResource", resourceName)

  resourceMethodMap = {ResourceMode.LOCK : lock,
                       ResourceMode.STOP_ON_DEMAND: stop_on_demand,
                       ResourceMode.PAUSE_ON_DEMAND: pause_on_demand,
                       ResourceMode.CALLBACK_ON_DEMAND: callback_on_demand}

  return resourceMethodMap[resourceType]

class behaviorWaiter_class(qicoreLegacy.BehaviorLegacy):
  def __init__(self, broker):
    qicoreLegacy.BehaviorLegacy.__init__(self, "behaviorWaiter")
    self.boxName = "behaviorWaiter"
    self.setName("behaviorWaiter")
    self.addInput("onDone")
    self.isComplete = False

  def onInput_onDone__(self, p):
    self.isComplete = True

  def waitForCompletion(self):
    while (self.isComplete == False):
      time.sleep(0.2)

class ConnectionType:
  INPUT=0
  OUTPUT=1
  PARAMETER=2

def safeOpen(filename):
  try:
    f = io.open(filename, encoding='utf-8', mode='r')
    return f
  except IOError as e:
    return None

class objectFactory:
  def __init__(self, folderName, broker):
    if (folderName != "" and not folderName.endswith("/")):
      folderName += "/"
    self._folderName = folderName
    self._broker = broker
    self._declaredObjects = set()
    self._boxDict = {}
    self._diagramDict= {}
    self._StateMachineDict = {}
    self._TransitionDict = {}
    self._StateDict = {}
    self._TimelineDict = {}
    self._connectionTypeForBox = {}

  def instanciateObjects(self, topdict, root = "root"):
    self.parseBox(root)
    for name,obj in self._boxDict.items():
      topdict[name] = obj
    return self._boxDict[root]

  def createWaiterOnBox(self, box, topdict):
    waiter = behaviorWaiter_class(self._broker)
    waiter.connectInput("onDone", box, "onStopped")
    self._boxDict[waiter.getName()] = waiter
    topdict[waiter.getName()] = waiter
    return waiter

  def parseTimeline(self, boxName, parentName):
    if (boxName in self._declaredObjects):
      return

    self._declaredObjects.add(boxName)
    timelineFile = safeOpen(self._folderName + boxName + ".xml")
    if (timelineFile == None):
      return
    timelineFile.close()

    timelineObject = qicore.Timeline(self._broker.getALBroker())
    timelineObject.loadFromFile(str(self._folderName + boxName + ".xml"))
    self._boxDict[parentName].setTimeline(timelineObject)
    self._TimelineDict[boxName] = timelineObject

  def parseDiagram(self, boxName):
    boxesInDiagram = set()

    dom = xml.dom.minidom.parse(self._folderName + boxName + ".xml")
    root = dom.getElementsByTagName('Diagram')[0]

    for obj in root.getElementsByTagName("Object"):
      objname = obj.attributes["Name"].value
      boxesInDiagram.add(objname)

    if (boxName in self._declaredObjects):
      return boxesInDiagram
    self._declaredObjects.add(boxName)

    for link in root.getElementsByTagName("Link"):
      inputObject = link.attributes["InputObject"].value
      outputObject = link.attributes["OutputObject"].value
      inputName = link.attributes["InputName"].value
      outputName = link.attributes["OutputName"].value
      self.parseBox(inputObject)
      self.parseBox(outputObject)

      if (self._connectionTypeForBox[inputObject][inputName] == ConnectionType.INPUT):
        self._boxDict[inputObject].connectInput(str(inputName), self._boxDict[outputObject], outputName)
      if (self._connectionTypeForBox[inputObject][inputName] == ConnectionType.OUTPUT):
        self._boxDict[inputObject].connectOutput(str(inputName), self._boxDict[outputObject], outputName)
      if (self._connectionTypeForBox[inputObject][inputName] == ConnectionType.PARAMETER):
        self._boxDict[inputObject].connectParameter(str(inputName), self._boxDict[OutputObject], outputName)

    return boxesInDiagram

  def parseStateMachine(self, boxName, parentBoxName):
    if (boxName in self._declaredObjects):
      return

    self._declaredObjects.add(boxName)

    stateMachineObject = qicore.StateMachine()

    try:
      dom = xml.dom.minidom.parse(self._folderName + boxName + ".xml")
    except IOError as e:
      return

    root = dom.getElementsByTagName('StateMachine')[0]
    for state in root.getElementsByTagName('State'):
      statename = state.attributes["Name"].value
      statename = boxName + "_" + statename
      objs = state.attributes["Objects"].value
      stateObject = qicore.State()
      objsList = objs.split(';')
      diagramObject = qicore.Diagram()
      for diag in objsList:
        if (diag != ""):
          objects_names = self.parseDiagram(diag)
          for box in objects_names:
            diagramObject.addBox(self._boxDict[box])
      stateObject.setDiagram(diagramObject)
      stateObject.setName(str(statename))
      stateMachineObject.addState(stateObject)
      self._diagramDict[statename + "_diagram"] = diagramObject
      self._StateDict[statename] = stateObject

    for tr in root.getElementsByTagName('Transition'):
      fromState = "_" + tr.attributes["From"].value
      toState = "_" + tr.attributes["To"].value
      timeOut = tr.attributes["TimeOut"].value
      transitionObject = qicore.Transition(self._StateDict[boxName + toState])
      transitionObject.setTimeOut(int(timeOut))
      self._StateDict[boxName + fromState].addTransition(transitionObject)
      self._TransitionDict[boxName + fromState + "__to__" + boxName + toState] = transitionObject

    for fstate in root.getElementsByTagName('FinalState'):
      stateMachineObject.setFinalState(self._StateDict[boxName + "_" + fstate.attributes["Name"].value])

    stateMachineObject.setInitialState(self._StateDict[boxName + "_" + root.getElementsByTagName('InitialState')[0].attributes["Name"].value])

    stateMachineObject.setName(str(boxName))
    self._boxDict[parentBoxName].setStateMachine(stateMachineObject)

    self._StateMachineDict[boxName] = stateMachineObject

  def formatParameter(self, name, content_type):
    if (content_type == ParameterType.STRING or content_type == ParameterType.RESOURCE):
      return "\"" + name + "\""
    else:
      return name

  def parseBox(self, boxName):
    if (boxName in self._declaredObjects):
      return

    self._declaredObjects.add(boxName)

    dom = xml.dom.minidom.parse(self._folderName + boxName + ".xml")
    root = dom.getElementsByTagName('Box')[0]



    module = __import__(boxName)
    boxClass = getattr(module, boxName + "_class")
    boxObject = boxClass(self._broker)
    self._boxDict[boxName] = boxObject

    boxObject.setPath(self._folderName)
    boxObject.registerOnLoadCallback(boxObject.__onLoad__)
    boxObject.registerOnUnloadCallback(boxObject.__onUnload__)

    connectionMap = {}

    for inp in root.getElementsByTagName('Input'):
      inpName = inp.attributes["name"].value
      inpNature = inp.attributes["nature"].value
      connectionMap[inpName] = ConnectionType.INPUT
      boxObject.addInput(inpName)
      f = generateInputMethod(int(inpNature), inpName)
      if (f != None):
        setattr(boxObject, "onInput_" + inpName + "__", types.MethodType(f, boxObject))

    # All boxes must have this input
    boxObject.addInput("onLoad")

    for out in root.getElementsByTagName('Output'):
      outName = out.attributes["name"].value
      outType = out.attributes["type"].value
      outNature = out.attributes["nature"].value
      connectionMap[outName] = ConnectionType.OUTPUT
      boxObject.addOutput(outName, int(outType == IOType.BANG))
      f = generateOutputMethod(int(outNature), outName)
      if (f != None):
        setattr(boxObject, outName, types.MethodType(f, boxObject))


    for param in root.getElementsByTagName('Parameter'):
      paramName = param.attributes["name"].value
      paramValue = param.attributes["value"].value
      paramContentType = param.attributes["content_type"].value
      paramInherits = param.attributes["inherits_from_parent"].value
      connectionMap[paramName] = ConnectionType.PARAMETER
      boxObject.addParameter(paramName, self.formatParameter(paramValue, paramContentType), int(paramInherits == 1))

    for res in root.getElementsByTagName("Resource"):
      resourceType = res.attributes["type"].value
      f = generateResourceMethod(int(resourceType))
      if (f != None):
        setattr(boxObject, "__onResource__", types.MethodType(f, boxObject))

    self._connectionTypeForBox[boxName] = connectionMap

    self.parseTimeline(boxName + "_timeline", boxName)
    self.parseStateMachine(boxName + "_state_machine", boxName)

