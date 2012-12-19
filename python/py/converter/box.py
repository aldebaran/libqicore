#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import converter.node as node
import converter.xar_types as xar_types

class Box(node.Node):

    def __init__(self, attrs):
        super(Box, self).__init__("Box")

        self.name = attrs.getValue("name")
        self.robot = attrs.getValue("robot")
        self.id_ = attrs.getValue("id")
        self.tooltip = attrs.getValue("tooltip")
        self.bitmap_expanded = attrs.getValue("bitmap_expanded")
        self.plugin = attrs.getValue("plugin")
        self.x_pos = attrs.getValue("x")
        self.y_pos = attrs.getValue("y")

        self.bitmap = ''
        self.script = xar_types.script()
        self.inputs = []
        self.outputs = []
        self.resources = []
        self.parameters = []
        self.child = None
        self.parent = None

        # Function map to speed up process
        self._function_map = { 'bitmap' : Box.attach_bitmap,
                               'script' : Box.attach_script,
                               'content' : Box.attach_script_content,
                               'Input' : Box.attach_input,
                               'Output' : Box.attach_output,
                               'Resource' : Box.attach_resource,
                               'Parameter': Box.attach_parameter,
                               'Box' : Box.attach_child,
                               'Diagram' : Box.attach_child,
                               'Timeline' : Box.attach_child }

    def attach_child(self, child):
        self.child = child

    def attach_bitmap(self, file_name):
        self.bitmap = file_name

    def attach_script_content(self, script):
        self.script.content = script

    def attach_script(self, attrs):
        self.script.language = attrs.getValue('language')

    def attach_input(self, attrs):
        self.inputs.append(xar_types.port(attrs.getValue('name'),
                                          attrs.getValue('type'),
                                          attrs.getValue('type_size'),
                                          attrs.getValue('nature'),
                                          attrs.getValue('stm_value_name'),
                                          attrs.getValue('inner'),
                                          attrs.getValue('tooltip'),
                                          attrs.getValue('id')))

    def attach_output(self, attrs):
        self.outputs.append(xar_types.port(attrs.getValue('name'),
                                           attrs.getValue('type'),
                                           attrs.getValue('type_size'),
                                           attrs.getValue('nature'),
                                           attrs.getValue('stm_value_name'),
                                           attrs.getValue('inner'),
                                           attrs.getValue('tooltip'),
                                           attrs.getValue('id')))

    def attach_resource(self, attrs):
        self.resources.append(xar_types.resource(attrs.getValue('name'),
                                                 attrs.getValue('type'),
                                                 attrs.getValue('timeout')))

    def attach_parameter(self, attrs):
        parameter = xar_types.parameter(attrs.getValue('name'),
                                        attrs.getValue('inherits_from_parent'),
                                        attrs.getValue('content_type'),
                                        attrs.getValue('value'),
                                        attrs.getValue('default_value'),
                                        attrs.getValue('min'),
                                        attrs.getValue('max'),
                                        attrs.getValue('tooltip'),
                                        attrs.getValue('id'),
                                        attrs.getValue('custom_choice'))
        self.parameters.append(parameter)

