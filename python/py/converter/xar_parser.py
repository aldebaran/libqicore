#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import codecs
import xml.sax

import converter.xar_types as xar_types
import converter.box as box
import converter.diagram as diagram
import converter.timeline as timeline
import converter.behavior_layer as b_layer
import converter.behavior_keyframe as b_keyframe

def generate_tree_from_filename(filename):
    """ Generates the reprenstation of the file designed by filename
        in memory. Build a tree.

        :param filename: name of the file to parse
        :returns: the tree representing the file
    """
    ofile = _check_open_file(filename)
    if not ofile:
        return None
    parser = xml.sax.make_parser()
    handler = XarHandler()
    parser.setContentHandler(handler)
    parser.parse(filename)
    return handler.get_root()

def _check_open_file(filename):
    with codecs.open(filename, encoding='utf-8', mode='r') as ofile:
        header = ofile.readline()
        header = header + ofile.readline()
        if (not ("xar_version=\"3\"" in header)):
            return False
        return True

class XarHandler(xml.sax.handler.ContentHandler):
    """ ContentHandler to parse the xar file
    """

    def __init__(self):
        xml.sax.handler.ContentHandler.__init__(self)
        self._nodes = []
        self._buffer = ""
        self._root = None

    def startElement(self, name, attrs):
        new_node = None
        if (name == 'Box'):
            new_node = box.Box(xar_types.attributes(attrs))
        elif (name == 'Diagram'):
            new_node = diagram.Diagram(xar_types.attributes(attrs))
        elif (name == 'Timeline'):
            new_node = timeline.Timeline(xar_types.attributes(attrs))
        elif (name == 'BehaviorLayer'):
            new_node = b_layer.BehaviorLayer(xar_types.attributes(attrs))
        elif (name == 'BehaviorKeyframe'):
            new_node = b_keyframe.BehaviorKeyFrame(xar_types.attributes(attrs))

        if new_node:
            if not self._root:
                self._root = new_node
            # Attach to parent here
            if self._nodes:
                self._nodes[len(self._nodes) - 1].attach_attribute(name,
                                                                   new_node)
            self._nodes.append(new_node)
        else:
            if (self._nodes and name != 'bitmap' and name != 'content'):
                self._nodes[len(self._nodes) - 1].attach_attribute(name,
                                                  xar_types.attributes(attrs))

    def endElement(self, name):
        if (name == 'Box' or name == 'Diagram' or name == 'Timeline'
              or name == 'BehaviorLayer' or name == 'BehaviorKeyframe'):
            self._nodes.pop()
        elif (name == 'bitmap' or name == 'content'):
            self._nodes[len(self._nodes) - 1].attach_attribute(name,
                                                               self._buffer)
        self._buffer = ''

    def characters(self, content):
        self._buffer += content

    def get_root(self):
        return self._root

