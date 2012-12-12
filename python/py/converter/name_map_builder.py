#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import string
import re
import os
import sys

def format_name(name):
    if not name:
        return "unnamed_object"

    result = ""
    for char in name:
        if char in string.printable:
            result += char

    name = result
    pattern = re.compile(r'[\W_]+')
    return pattern.sub('_', name)

def find_port_name(box, port_id):
    for port in box.inputs + box.outputs + box.parameters:
        if (port.id == port_id):
            return port.name
    return ""

def find_input_nature(box, port_id):
    for port in box.inputs:
        if (port.id == port_id):
            return port.nature
    return ""

class NameMapBuilder:

    def __init__(self):
        self._box_stack = []
        self._boxes = {}
        self._name_set = set()

    def construct_name(self):
        box = self._box_stack.pop()
        name = box.name
        self._box_stack.append(box)
        name = "l" + str(len(self._box_stack) - 1) + "_" + name

        return self.find_available_name(name)

    def find_available_name(self, name):
        box_id = 1
        if (name in self._name_set):
            currentname = name + "_" + str(box_id)
        else:
            self._name_set.add(name)
            return name

        # Name collision is ugly...
        while currentname in self._name_set:
            currentname = name + "_" + str(box_id)
            box_id = box_id + 1

        self._name_set.add(currentname)
        return currentname

    def get_name_map(self):
        return self._boxes

    def visit(self, node):
        if not node:
            return
        methname = "_visit_%s" % node.node_name.lower()
        method = getattr(self, methname, self.visit)
        return method(node)

    def _visit_timeline(self, node):
        box = self._box_stack.pop()
        self._box_stack.append(box)
        node.name = box.name
        for layer in node.behavior_layers:
            self.visit(layer)

    def _visit_diagram(self, node):
        id_map = {}
        for child in node.boxes:
            self.visit(child)
            id_map[child.id_] = child.name
        parent_name = ""
        if (node.boxes and node.boxes[0].parent != None):
            parent_name = node.boxes[0].parent.name
        elif (len(self._box_stack) != 0):
            parent_name = self._box_stack[len(self._box_stack) - 1].name
        else:
            sys.stderr.write("ERROR: No parent for " + node.name
                              + " abort..." + os.linesep)
            sys.exit(2)
        id_map[str(0)] = parent_name

        # Diagram has no name in legacy format so we fill it
        node.name = self.find_available_name(parent_name + "_diagram")

        for link in node.links:
            link.inputowner = id_map[link.inputowner]
            link.outputowner = id_map[link.outputowner]

            link.inputName = find_port_name(self._boxes[link.inputowner],
                                                  link.indexofinput)
            link.outputName = find_port_name(self._boxes[link.outputowner],
                                                  link.indexofoutput)

    def _visit_box(self, node):
        if self._box_stack:
            node.parent = self._box_stack.pop()
            self._box_stack.append(node.parent)
        self._box_stack.append(node)
        node.name = format_name(node.name)
        node.name = self.construct_name()

        self._boxes[node.name] = node

        self.visit(node.child)
        self._box_stack.pop()

    def _visit_behaviorlayer(self, node):
        node.name = format_name(node.name)
        for keyframe in node.behavior_keyframes:
            self.visit(keyframe)

    def _visit_behaviorkeyframe(self, node):
        node.name = format_name(node.name)
        if node.child:
            self.visit(node.child)

