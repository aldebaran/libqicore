#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import re

import code_patcher

class nameMapBuilder:
  def __init__(self):
    self._namesStack = []
    self._boxStack = []
    self._boxes = {}

  def find_port_name(self, box, portId):
    for port in box.inputs + box.outputs + box.parameters:
      if (port.id == portId):
        return port.name
    return ""

  def find_input_nature(self, box, portId):
    for port in box.inputs:
      if (port.id == portId):
        return port.nature
    return ""

  def formatName(self, name):
    pattern = re.compile('[\W_]+')
    return pattern.sub('_', name)

  def constructName(self):
    result = ""
    for name in self._namesStack:
      result = result + name + "_"
    return result

  def getNameMap(self):
    return self._boxes

  def visit(self, node):
    if (node == None):
      return
    methname = "visit_%s" % node.nodeName
    method = getattr(self, methname, self.visit)
    return method(node)

  def visit_Timeline(self, node):
    node.name = self.constructName()
    node.name = self.formatName(node.name)
    for layer in node.behaviorLayers:
      self.visit(layer)

  def fix_onLoad(self, node, link, idMap):
    # Change link: box will start when loaded
    # and not when its parent is loaded
    link.outputowner = node.name

    for inp in node.inputs:
      if (inp.nature == code_patcher.InputType.ONLOAD):
        link.indexofoutput = inp.id

  def visit_Diagram(self, node):
    # Diagram has no name in legacy format so we fill it
    node.name = self.constructName() + "diagram"
    node.name = self.formatName(node.name)
    idMap = {}
    for child in node.boxes:
      self.visit(child)
      idMap[child.id_] = child.name
    parentName = ""
    if (node.boxes and node.boxes[0].parent != None):
      parentName = node.boxes[0].parent.name
    elif (len(self._boxStack) != 0):
      parentName = self._boxStack[len(self._boxStack) - 1].name
    else:
      print("ERROR: No parent for ", node.name, " abort...")
      sys.exit(2)
    idMap[str(0)] = parentName

    for link in node.links:
      link.inputowner = idMap[link.inputowner]
      link.outputowner = idMap[link.outputowner]

      # Detect a frameManager hack
      if (link.outputowner == parentName):
        if (self.find_input_nature(self._boxes[link.outputowner], link.indexofoutput) == str(code_patcher.InputType.ONLOAD)):
          self.fix_onLoad(self._boxes[link.inputowner], link, idMap)

      link.inputName = self.find_port_name(self._boxes[link.inputowner], link.indexofinput)
      link.outputName = self.find_port_name(self._boxes[link.outputowner], link.indexofoutput)

  def visit_Box(self, node):
    if (len(self._boxStack) != 0):
      node.parent = self._boxStack.pop()
      self._boxStack.append(node.parent)
    self._boxStack.append(node)
    node.name = self.formatName(node.name)
    simpleName = node.name
    node.name = self.constructName() + node.name
    self._namesStack.append(simpleName)

    self._boxes[node.name] = node

    self.visit(node.child)
    self._namesStack.pop()
    self._boxStack.pop()

  def visit_BehaviorLayer(self, node):
    node.name = self.formatName(node.name)
    self._namesStack.append(node.name)
    for keyframe in node.behaviorKeyFrames:
      self.visit(keyframe)
    self._namesStack.pop()

  def visit_BehaviorKeyFrame(self, node):
    node.name = self.formatName(node.name)
    self._namesStack.append(node.name)
    if (node.child != None):
      self.visit(node.child)
    self._namesStack.pop()

