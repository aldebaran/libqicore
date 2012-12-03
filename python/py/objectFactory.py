#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import sys
import os
import io
import time
import xml.dom.minidom

import qicore
import qicoreLegacy
from converter.xar_types import *

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
      connectionMap[inpName] = ConnectionType.INPUT
      boxObject.addInput(inpName)

    for out in root.getElementsByTagName('Output'):
      outName = out.attributes["name"].value
      outType = out.attributes["type"].value
      connectionMap[outName] = ConnectionType.OUTPUT
      boxObject.addOutput(outName, int(outType == IOType.BANG))


    for param in root.getElementsByTagName('Parameter'):
      paramName = param.attributes["name"].value
      paramValue = param.attributes["value"].value
      paramContentType = param.attributes["content_type"].value
      paramInherits = param.attributes["inherits_from_parent"].value
      connectionMap[paramName] = ConnectionType.PARAMETER
      boxObject.addParameter(paramName, self.formatParameter(paramValue, paramContentType), int(paramInherits == 1))

    self._connectionTypeForBox[boxName] = connectionMap

    self.parseTimeline(boxName + "_timeline", boxName)
    self.parseStateMachine(boxName + "_state_machine", boxName)

