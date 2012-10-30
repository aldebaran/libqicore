#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import sys
import os
import xml.dom.minidom

import code_patcher

def safeOpen(filename):
  try:
    f = open(filename)
    return f
  except IOError as e:
    return None

class behaviorParser:
  def __init__(self, folderName, outputFile):
    self._outputFile = outputFile
    self._folderName = folderName
    self._declaredObjects = set()
    self._declarationsStr = "#" + "-" * 30 + "Declarations" + "-" * 30 + os.linesep
    self._instanciationsStr = "#" + "-" * 30 + "Instanciations" + "-" * 30 + os.linesep
    self._connectionsStr = "#" + "-" * 30 + "Connections" + "-" * 30 + os.linesep
    self._runStr = "#" + "-" * 30 + "Run" + "-" * 30 + os.linesep

  def addBoxToInstanciationStr(self, box):
    code = box + " = " + box + "_class()" + os.linesep
    self._instanciationsStr = self._instanciationsStr + code + os.linesep

  def generatePython(self):
    # TODO: Change IP here to a generic value
    self._instanciationsStr = (self._instanciationsStr
                            + "broker = naoqi.ALBroker(\"pythonBroker\", \"0.0.0.0\", 9600, \"127.0.0.1\", 9559)"
                            + os.linesep
                            + "naoqi.ALProxy.initProxies()"
                            + os.linesep + os.linesep)
    self.parseBox("root")
    self._runStr += ("root.__onLoad__()" + os.linesep
                      + "root.onInput_onStart__(None)" + os.linesep
                      + "if (root.hasTimeline()):" + os.linesep
                      + "  root.getTimeline().waitForTimelineCompletion()")
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
                    + " = qicore.Timeline(broker.getBroker())"
                    + os.linesep
                    + boxName + ".loadFromFile(\""
                    + fullPath + "\")" + os.linesep
                    + parentName + ".setTimeline(" + boxName + ")"
                    + os.linesep + os.linesep)
    self._instanciationsStr = self._instanciationsStr + timelineCode

  def parseDiagram(self, boxName):
    boxesName = set()
    if (boxName in self._declaredObjects):
      return

    self._declaredObjects.add(boxName)
    diagramFile = safeOpen(self._folderName + boxName + ".xml")
    if (diagramFile == None):
      print("No " + boxName + " in folder, abort...")
      sys.exit(2)

    dom = xml.dom.minidom.parse(diagramFile)
    root = dom.getElementsByTagName('Diagram')[0]

    for obj in root.getElementsByTagName("Object"):
      objname = obj.attributes["Name"].value
      boxesName.add(objname)

    for link in root.getElementsByTagName("Link"):
      inputObject = link.attributes["InputObject"].value
      outputObject = link.attributes["OutputObject"].value
      inputName = link.attributes["InputName"].value
      outputName = link.attributes["OutputName"].value
      self.parseBox(inputObject)
      self.parseBox(outputObject)
      # FIXME: use connnectOutput sometimes
      self._connectionsStr += (inputObject + ".connectInput(\"" + inputName + "\", \""
                                + outputObject + "__" + outputName + "\", True)"
                                + os.linesep)

    diagramFile.close()
    return boxesName;

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
      self._instanciationsStr += (boxName + ".addState("
                                  + boxName + "_" + statename + "" ")" + os.linesep)

    for tr in root.getElementsByTagName('Transition'):
      print(tr.toxml())

    for fstate in root.getElementsByTagName('FinalState'):
      self._instanciationsStr += (boxName + ".setFinalState("
                                  + boxName +  "_" + fstate.attributes["Name"].value
                                  + ")" + os.linesep)

    self._instanciationsStr += (boxName + ".setInitialState("
                              + boxName + "_" + root.getElementsByTagName('InitialState')[0].attributes["Name"].value
                              + ")" + os.linesep + os.linesep)

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

    pyFile = safeOpen(self._folderName + boxName + ".py")
    if (pyFile != None):
      code = pyFile.read()
      patcher = code_patcher.patcher(boxName, code, root)
      code = patcher.patch()
      self._declarationsStr = self._declarationsStr + code
      self.addBoxToInstanciationStr(boxName)
      pyFile.close()


    self.parseTimeline(boxName + "_timeline", boxName)
    self.parseStateMachine(boxName + "_state_machine", boxName)
    xmlFile.close()
