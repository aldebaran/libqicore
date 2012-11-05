#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

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
    node.name = self.constructName() + "timeline"
    for layer in node.behaviorLayers:
      self.visit(layer)

  def visit_Diagram(self, node):
    # Diagram has no name in legacy format so we fill it
    node.name = self.constructName() + "diagram"
    idMap = {}
    for child in node.boxes:
      self.visit(child)
      idMap[child.id_] = child.name
    if (node.boxes[0].parent != None):
      idMap[str(0)] = node.boxes[0].parent.name

    for link in node.links:
      # Detect a frameManager hack

      link.inputowner = idMap[link.inputowner]
      link.inputName = self.find_port_name(self._boxes[link.inputowner], link.indexofinput)
      link.outputowner = idMap[link.outputowner]
      link.outputName = self.find_port_name(self._boxes[link.outputowner], link.indexofoutput)

  def visit_Box(self, node):
    if (len(self._boxStack) != 0):
      node.parent = self._boxStack.pop()
      self._boxStack.append(node.parent)
    self._boxStack.append(node)
    node.name = node.name.replace(" ", "_")
    simpleName = node.name
    node.name = self.constructName() + node.name
    self._namesStack.append(simpleName)

    self._boxes[node.name] = node

    self.visit(node.child)
    self._namesStack.pop()

  def visit_BehaviorLayer(self, node):
    self._namesStack.append(node.name)
    for keyframe in node.behaviorKeyFrames:
      self.visit(keyframe)
    self._namesStack.pop()

  def visit_BehaviorKeyFrame(self, node):
    self._namesStack.append(node.name)
    if (node.child != None):
      self.visit(node.child)
    self._namesStack.pop()
