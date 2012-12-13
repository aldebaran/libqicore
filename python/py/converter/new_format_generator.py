#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import codecs
import os

import converter.code_patcher as code_patcher
import converter.file_writer as file_writer

def _build_objects_list(state):
    result = ""
    for obj in state.objects:
        result = result + obj.name + ";"
    return result

class Interval:

    def __init__(self, name, begin, end, obj):
        self.name = name
        self.begin = int(begin)
        self.end = int(end)
        self.obj = obj

class State:

    def __init__(self):
        self.begin = -1
        self.end = -1
        self.labels = []
        self.obj_nb = 0
        self.objects = []

class NewFormatGenerator:

    def __init__(self, boxes):
        self._boxes = boxes

    def generate_main(self):
        with codecs.open("main.py", encoding='utf-8', mode='w') as fmain:
            file_writer.write_main(fmain)

    def visit(self, node):
        if not node:
            return
        methname = "_visit_%s" % node.node_name.lower()
        method = getattr(self, methname, self.visit)
        return method(node)

    def _visit_timeline(self, node):
        if (node.behavior_layers):
            self._convert_timelinelayers(node)

        if (node.actuator_list):
            with codecs.open(node.name + "_timeline.xml",
                              encoding='utf-8', mode='w') as fti:
                file_writer.write_actuatorList(fti, node)

    def _visit_box(self, node):
        with codecs.open(node.name + ".py",
                         encoding='utf-8', mode='w') as fpy:
            script = code_patcher.patch(node)
            fpy.write(script + os.linesep)

        with codecs.open(node.name + '.xml',
                         encoding='utf-8', mode='w') as fxml:
            file_writer.write_box_meta(fxml, node)

        self.visit(node.child)

    def _visit_diagram(self, node):
        for child in node.boxes:
            self.visit(child)

    def _convert_timelinelayers(self, node):
        last_frame = -1
        interval_list = []
        for layer in node.behavior_layers:

            for i in range(len(layer.behavior_keyframes)):
                keyframe = layer.behavior_keyframes[i]
                if (i != (len(layer.behavior_keyframes) - 1)):
                    nextframe = layer.behavior_keyframes[i + 1]
                    interval_list.append(Interval(keyframe.name,
                                                  keyframe.index,
                                                  nextframe.index,
                                                  keyframe.child))
                else:
                    last_frame = max(last_frame, int(keyframe.index))
                    interval_list.append(Interval(keyframe.name,
                                                  keyframe.index,
                                                  -1,
                                                  keyframe.child))

                self.visit(keyframe.child)

        interval_list = sorted(interval_list, key=lambda inter: inter.begin)

        # Change max value of every interval
        # Assume that last state is one frame long... ?
        last_frame = last_frame + 1
        for inter in interval_list:
            if (inter.end == -1):
                inter.end = last_frame

        # Debug purpose
        # print("----------------- Intervals --------------------")
        # for inter in interval_list:
          #  print(inter.name, " : ", inter.begin, " -> ", inter.end)

        state_list = self._convert_to_statemachine(interval_list, last_frame)

        if (node.fps == None):
            node.fps = 25

        with codecs.open(node.name + "_state_machine.xml",
                         encoding='utf-8', mode='w') as fst:
            file_writer.write_state_machine(fst, node.name,
                                            state_list, node.fps)

    def _convert_to_statemachine(self, interval_list, end_frame):
        if (len(interval_list) == 0):
            return []

        state_list = []
        current_inter_list = []
        current_inter_list.append(interval_list.pop(0))

        while current_inter_list:
            current_state_begin = (max(current_inter_list,
                                   key=lambda inter: inter.begin)).begin
            current_state_end = 0
            current_state_labels = []
            next_state_begin = end_frame

            last_inter = current_inter_list.pop()
            current_inter_list.append(last_inter)
            current_state_labels.append(last_inter.name)

            # Retrieve all intervals that starts with the same x
            # Take the start of the next interval as well
            while interval_list:
                next_inter = interval_list.pop(0)
                if (next_inter.begin == current_state_begin):
                    current_state_labels.append(next_inter.name)
                    current_inter_list.append(next_inter)
                else:
                    interval_list.insert(0, next_inter)
                    next_state_begin = next_inter.begin
                    break

            current_state_end = (min(current_inter_list,
                                 key=lambda inter: inter.end)).end

            # Create the new state with data
            current_state = State()
            current_state.begin = current_state_begin
            current_state.end = min(current_state_end, next_state_begin)
            current_state.obj_nb = len(current_inter_list)
            current_state.labels = current_state_labels

            for inter in current_inter_list:
                (current_state.objects).append(inter.obj)

            state_list.append(current_state)

            if (next_state_begin < current_state_end):
                # Take new interval if needed
                current_inter_list.append(interval_list.pop(0))
            else:
                # Clean intervals no more useful
                current_inter_list = [x for x in current_inter_list
                                      if not (x.end == current_state_end)]
                if ((current_state_end == next_state_begin)
                      and (len(interval_list) != 0)):
                    current_inter_list.append(interval_list.pop(0))
                if ((not current_inter_list) and interval_list):
                    current_inter_list.append(interval_list.pop(0))

        # Debug purpose
        # print("------------------ States ----------------------")
        # for st in state_list:
            # print("State : ", st.begin, " -> ", st.end, ", with : ",
            #       st.obj_nb, "keyframes, labels : ", st.labels)

        return state_list

