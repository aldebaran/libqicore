#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import xml.sax

import xar_types
import box
import diagram
import timeline
import behaviorLayer
import behaviorKeyFrame

def parse(file):
  parser = xml.sax.make_parser()
  handler = xarHandler()
  parser.setContentHandler(handler)
  parser.parse(file)
  return handler.getRoot()

class xarHandler(xml.sax.handler.ContentHandler):
  def __init__(self):
    xml.sax.handler.ContentHandler.__init__(self)
    self._nodes = []
    self._buffer = ""
    self._root = None

  def startElement(self, name, attrs):
    newNode = None;
    if (name == 'Box'):
      newNode = box.box(xar_types.attributes(attrs))
    elif (name == 'Diagram'):
      newNode = diagram.diagram(xar_types.attributes(attrs))
    elif (name == 'Timeline'):
      newNode = timeline.timeline(xar_types.attributes(attrs))
    elif (name == 'BehaviorLayer'):
      newNode = behaviorLayer.behaviorLayer(xar_types.attributes(attrs))
    elif (name == 'BehaviorKeyframe'):
      newNode = behaviorKeyFrame.behaviorKeyFrame(xar_types.attributes(attrs))

    if (newNode != None):
      if (self._root == None):
        self._root = newNode
      print('-' * len(self._nodes), '> ', end = '')
      newNode.printName()
      #Attach to parent here
      if (len(self._nodes) != 0):
        self._nodes[len(self._nodes) - 1].attachAttribute(name, newNode)
      self._nodes.append(newNode)
    else:
      if (len(self._nodes) != 0 and name != 'bitmap' and name != 'content'):
        self._nodes[len(self._nodes) - 1].attachAttribute(name, xar_types.attributes(attrs))

  def endElement(self, name):
    if (name == 'Box' or name == 'Diagram' or name == 'Timeline' or name == 'BehaviorLayer' or name == 'BehaviorKeyframe'):
      current = self._nodes.pop()
    elif (name == 'bitmap' or name == 'content'):
      self._nodes[len(self._nodes) - 1].attachAttribute(name, self._buffer)
    self._buffer = ''

  def characters(self, content):
    self._buffer += content;

  def getRoot(self):
    return self._root
