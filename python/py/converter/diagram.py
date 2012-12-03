#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import node
import xar_types

class diagram(node.node):
  def __init__(self, attrs):
    node.node.__init__(self, "Diagram")

    self.scale = attrs.getValue('scale')

    self.boxes = []
    self.links = []
    self.name = ""

    self._functionMap = { 'Box' : diagram.attachBox,
                          'Link' : diagram.attachLink}

  def attachBox(self, box):
    self.boxes.append(box)

  def attachLink(self, attrs):
    self.links.append(xar_types.link(attrs.getValue("inputowner"),
                                      attrs.getValue("indexofinput"),
                                      attrs.getValue("outputowner"),
                                      attrs.getValue("indexofoutput")))
