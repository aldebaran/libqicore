#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import sys
import os
import xml.dom.minidom

class ConnectionType:
  INPUT=0
  OUTPUT=1
  PARAMETER=2

def safeOpen(filename):
  try:
    f = open(filename, encoding='utf-8', mode='r')
    return f
  except IOError as e:
    return None

class behaviorParser:
  def __init__(self, folderName, outputFile):
    self._outputFile = outputFile
    self._folderName = folderName
    self._declaredObjects = set()
    self._connectionTypeForBox = {}
    self._declarationsStr = "#" + "-" * 30 + "Declarations" + "-" * 30 + os.linesep
    self._instanciationsStr = "#" + "-" * 30 + "Instanciations" + "-" * 30 + os.linesep
    self._connectionsStr = "#" + "-" * 30 + "Connections" + "-" * 30 + os.linesep
    self._runStr = "#" + "-" * 30 + "Run" + "-" * 30 + os.linesep

  def addBoxToInstanciationStr(self, box):
    code = box + " = " + box + "_class()" + os.linesep
    self._instanciationsStr = self._instanciationsStr + code + os.linesep
    self._connectionsStr += (box + ".connectInput(\"onLoad\", \""
                                + box + "____Internal__OnLoad\"" + ", True)"
                                + os.linesep)

  def buildRunStr(self):
    self._runStr += ("class waiter_class(qicoreLegacy.BehaviorLegacy):" + os.linesep
                    + "  def __init__(self):" + os.linesep
                    + "    qicoreLegacy.BehaviorLegacy.__init__(self, \"waiter\", True)" + os.linesep
                    + "    self.boxName = \"waiter\"" + os.linesep
                    + "    self.setName(\"waiter\")" + os.linesep
                    + "    self.setBroker(broker.getALBroker())" + os.linesep
                    + "    self.BIND_PYTHON(self.getName(), \"onInput_onDone__\", 1)" + os.linesep
                    + "    self.addInput(\"onDone\")" + os.linesep
                    + "    self.isComplete = False" + os.linesep
                    + "  def onInput_onDone__(self, p):" + os.linesep
                    + "    self.isComplete = True" + os.linesep
                    + "  def waitForCompletion(self):" + os.linesep
                    + "    while (self.isComplete == False):" + os.linesep
                    + "      time.sleep(0.2)" + os.linesep * 2
                    + "waiter = waiter_class()" + os.linesep
                    + "waiter.connectInput(\"onDone\", \"root__onStopped\", True)" + os.linesep
                    + "root.__onLoad__()" + os.linesep
                    + "root.onInput_onStart__(None)" + os.linesep
                    + "waiter.waitForCompletion()" + os.linesep)

  def generatePython(self):
    # TODO: Change IP here to a generic value
    self._instanciationsStr = (self._instanciationsStr
                            + "broker = naoqi.ALBroker(\"pythonBroker\", \"0.0.0.0\", 9600, \"127.0.0.1\", 9559)"
                            + os.linesep
                            + "naoqi.ALProxy.initProxies()"
                            + os.linesep + os.linesep)
    self.parseBox("root")
    self.buildRunStr()
    self._outputFile.write("#!/usr/bin/env python" + os.linesep
                            + "# -*- coding: utf-8 -*-" + os.linesep
                            + os.linesep + os.linesep
                            + "import naoqi" + os.linesep
                            + "from naoqi import *"
                            + os.linesep
                            + "import qicore" + os.linesep
                            + "import qicoreLegacy"
                            + os.linesep + os.linesep
                            + self._declarationsStr
                            + os.linesep
                            + self._instanciationsStr
                            + os.linesep
                            + self._connectionsStr
                            + os.linesep
                            + self._runStr)

  def parseTimeline(self, boxName, parentName):
    if (boxName in self._declaredObjects):
      return

    self._declaredObjects.add(boxName)
    #FIXME: May be there is a better way to do that
    fullPath = os.path.abspath((self._folderName + boxName + ".xml"))
    timelineFile = safeOpen(fullPath)
    if (timelineFile == None):
      return
    timelineFile.close()

    timelineCode = (boxName
                    + " = qicore.Timeline(broker.getALBroker())"
                    + os.linesep
                    + boxName + ".loadFromFile(\""
                    + fullPath + "\")" + os.linesep
                    + parentName + ".setTimeline(" + boxName + ")"
                    + os.linesep + os.linesep)
    self._instanciationsStr = self._instanciationsStr + timelineCode

  def parseDiagram(self, boxName):
    boxesInDiagram = set()
    if (boxName in self._declaredObjects):
      return boxesInDiagram

    self._declaredObjects.add(boxName)
    diagramFile = safeOpen(self._folderName + boxName + ".xml")
    if (diagramFile == None):
      print("No " + boxName + " in folder, abort...")
      sys.exit(2)

    dom = xml.dom.minidom.parse(diagramFile)
    root = dom.getElementsByTagName('Diagram')[0]

    for obj in root.getElementsByTagName("Object"):
      objname = obj.attributes["Name"].value
      boxesInDiagram.add(objname)

    for link in root.getElementsByTagName("Link"):
      inputObject = link.attributes["InputObject"].value
      outputObject = link.attributes["OutputObject"].value
      inputName = link.attributes["InputName"].value
      outputName = link.attributes["OutputName"].value
      self.parseBox(inputObject)
      self.parseBox(outputObject)

      connectionTypeStr = ""
      if (self._connectionTypeForBox[inputObject][inputName] == ConnectionType.INPUT):
        connectionTypeStr = ".connectInput(\""
      if (self._connectionTypeForBox[inputObject][inputName] == ConnectionType.OUTPUT):
        connectionTypeStr = ".connectOutput(\""
      if (self._connectionTypeForBox[inputObject][inputName] == ConnectionType.PARAMETER):
        connectionTypeStr = ".connectParameter(\""
      self._connectionsStr += (inputObject + connectionTypeStr + inputName + "\", \""
                                + outputObject + "__" + outputName + "\", True)"
                                + os.linesep)

    diagramFile.close()
    return boxesInDiagram

  def parseStateMachine(self, boxName, parentBoxName):
    if (boxName in self._declaredObjects):
      return

    self._declaredObjects.add(boxName)
    stateMachineFile = safeOpen(self._folderName + boxName + ".xml")
    if (stateMachineFile == None):
      return

    self._instanciationsStr += (boxName + " = qicore.StateMachine()" + os.linesep)

    dom = xml.dom.minidom.parse(stateMachineFile)

    root = dom.getElementsByTagName('StateMachine')[0]
    for state in root.getElementsByTagName('State'):
      statename = state.attributes["Name"].value
      self._instanciationsStr += (boxName + "_" + statename + " = qicore.State()" + os.linesep )
      objs = state.attributes["Objects"].value
      objsList = objs.split(';')
      self._instanciationsStr += (boxName + "_" + statename + "_diagram = qicore.Diagram()" + os.linesep)
      for diag in objsList:
        if (diag != ""):
          objects_names = self.parseDiagram(diag)
          for box in objects_names:
            self._instanciationsStr += (boxName + "_" + statename + "_diagram.addBox(" + box + ")" + os.linesep)
      self._instanciationsStr += (boxName + "_" + statename + ".setDiagram("
                                  + boxName + "_" + statename + "_diagram" ")" + os.linesep)
      self._instanciationsStr += (boxName + ".addState(" + boxName + "_" + statename + ")" + os.linesep)
      self._instanciationsStr += (boxName + "_" + statename +  ".setName(\"" + boxName + "_" + statename + "\")" + os.linesep)

    for tr in root.getElementsByTagName('Transition'):
      fromState = "_" + tr.attributes["From"].value
      toState = "_" + tr.attributes["To"].value
      timeOut = tr.attributes["TimeOut"].value
      self._instanciationsStr += (boxName + fromState + "__to__" + boxName + toState + " = qicore.Transition(" + boxName + toState + ")" + os.linesep
                                  + boxName + fromState + "__to__" + boxName + toState + ".setTimeOut(" + timeOut + ")" + os.linesep
                                  + boxName + fromState + ".addTransition(" + boxName + fromState + "__to__" + boxName + toState + ")" + os.linesep)

    for fstate in root.getElementsByTagName('FinalState'):
      self._instanciationsStr += (boxName + ".setFinalState("
                                  + boxName +  "_" + fstate.attributes["Name"].value
                                  + ")" + os.linesep)

    self._instanciationsStr += (boxName + ".setInitialState("
                              + boxName + "_" + root.getElementsByTagName('InitialState')[0].attributes["Name"].value
                              + ")" + os.linesep + os.linesep)
    self._instanciationsStr += (boxName + ".setName(\"" + boxName + "\")" + os.linesep)
    self._instanciationsStr += (parentBoxName + ".setStateMachine(" + boxName + ")" + os.linesep)

    stateMachineFile.close()

  def parseBox(self, boxName):
    if (boxName in self._declaredObjects):
      return

    self._declaredObjects.add(boxName)
    xmlFile = safeOpen(self._folderName + boxName + ".xml")
    if (xmlFile == None):
      print("No " + boxName + " in folder, abort...")
      sys.exit(2)

    dom = xml.dom.minidom.parse(xmlFile)
    root = dom.getElementsByTagName('Box')[0]

    connectionMap = {}

    for inp in root.getElementsByTagName('Input'):
      inpName = inp.attributes["name"].value
      connectionMap[inpName] = ConnectionType.INPUT

    for out in root.getElementsByTagName('Output'):
      outName = out.attributes["name"].value
      connectionMap[outName] = ConnectionType.OUTPUT

    for param in root.getElementsByTagName('Parameter'):
      paramName = param.attributes["name"].value
      connectionMap[paramName] = ConnectionType.PARAMETER

    self._connectionTypeForBox[boxName] = connectionMap

    pyFile = safeOpen(self._folderName + boxName + ".py")
    if (pyFile != None):
      code = pyFile.read()
      self._declarationsStr = self._declarationsStr + code
      self.addBoxToInstanciationStr(boxName)
      pyFile.close()


    self.parseTimeline(boxName + "_timeline", boxName)
    self.parseStateMachine(boxName + "_state_machine", boxName)
    xmlFile.close()
