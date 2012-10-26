#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import node
import xar_types

class box(node.node):
  def __init__(self, attrs):
    node.node.__init__(self, "Box")

    self.name = attrs.getValue("name")
    self.robot = attrs.getValue("robot")
    self.id_ = attrs.getValue("id")
    self.tooltip = attrs.getValue("tooltip")
    self.bitmap_expanded = attrs.getValue("bitmap_expanded")
    self.plugin = attrs.getValue("plugin")
    self.x = attrs.getValue("x")
    self.y = attrs.getValue("y")

    self.bitmap = ''
    self.script = xar_types.script()
    self.inputs = []
    self.outputs = []
    self.resources = []
    self.parameters = []
    self.child = None
    # Function map to speed up process
    self._functionMap = { 'bitmap' : box.attachBitmap,
                          'script' : box.attachScript,
                          'content' : box.attachScriptContent,
                          'Input' : box.attachInput,
                          'Output' : box.attachOutput,
                          'Resource' : box.attachResource,
                          'Parameter': box.attachParameter,
                          'Box' : box.attachChild,
                          'Diagram' : box.attachChild,
                          'Timeline' : box.attachChild }

  def attachChild(self, child):
    self.child = child

  def attachBitmap(self, fileName):
    self.bitmap = fileName

  def attachScriptContent(self, script):
    self.script.content = script

  def attachScript(self, attrs):
    self.script.language = attrs.getValue('language')

  def attachInput(self, attrs):
    self.inputs.append(xar_types.port(attrs.getValue('name'),
                                       attrs.getValue('type'),
                                       attrs.getValue('type_size'),
                                       attrs.getValue('nature'),
                                       attrs.getValue('inner'),
                                       attrs.getValue('tooltip'),
                                       attrs.getValue('id')))

  def attachOutput(self, attrs):
    self.outputs.append(xar_types.port(attrs.getValue('name'),
                                        attrs.getValue('type'),
                                        attrs.getValue('type_size'),
                                        attrs.getValue('nature'),
                                        attrs.getValue('inner'),
                                        attrs.getValue('tooltip'),
                                        attrs.getValue('id')))

  def attachResource(self, attrs):
    self.resources.append(xar_types.resource(attrs.getValue('name'),
                                        attrs.getValue('type'),
                                        attrs.getValue('timeout')))

  def attachParameter(self, attrs):
    self.parameters.append(xar_types.parameter(attrs.getValue('name'),
                                          attrs.getValue('inherits_from_parent'),
                                          attrs.getValue('content_type'),
                                          attrs.getValue('value'),
                                          attrs.getValue('default_value'),
                                          attrs.getValue('min'),
                                          attrs.getValue('max'),
                                          attrs.getValue('tooltip'),
                                          attrs.getValue('id'),
                                          attrs.getValue('custom_choice')))
