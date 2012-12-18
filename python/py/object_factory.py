#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import codecs
import time
import xml.dom.minidom

import qicore
import qicore_legacy
import function_generator as f_gen
from converter.xar_types import ConnectionType, ParameterType, IOType, OutputType

class BehaviorWaiterClass(qicore_legacy.BehaviorLegacy):

    def __init__(self):
        qicore_legacy.BehaviorLegacy.__init__(self, "behaviorWaiter")
        self.addInput("onDone")
        self.is_complete = False

    def onInput_onDone__(self, param):
        self.is_complete = True

    def wait_for_completion(self):
        while not self.is_complete:
            time.sleep(0.2)

def _convert_parameter(value, content_type):
    if (content_type == ParameterType.STRING
        or content_type == ParameterType.RESOURCE):
        return value.encode("ascii", "ignore")
    elif (content_type == ParameterType.BOOL):
        return value == "True"
    elif (content_type == ParameterType.INT):
        return int(value)
    elif (content_type == ParameterType.DOUBLE):
        return float(value)
    else:
        return value

class StateClass(qicore_legacy.BehaviorLegacy):

    def __init__(self, name):
        qicore_legacy.BehaviorLegacy.__init__(self,
                                              name.encode('ascii', 'ignore'))
        self._box_list = []
        self._connection_list = []

    def onLoad(self):
        # Create connections local to the state
        for func in self._connection_list:
            func(True)

        for box in self._box_list:
            box.__onLoad__()

    def onUnload(self):
        # Destroy connections local to the state
        for func in self._connection_list:
            func(False)

        for box in self._box_list:
            box.__onUnload__()

    def add_box(self, box):
        self._box_list.append(box)

    def add_connection(self, connector):
        self._connection_list.append(connector)

class ObjectFactory(object):

    def __init__(self, folder_name, broker):
        if (folder_name != "" and not folder_name.endswith("/")):
            folder_name += "/"
        self._folder_name = folder_name
        self._broker = broker
        self._declared_objects = set()
        self._declared_links = set()
        self._box_dict = {}
        self._statemachine_dict = {}
        self._transition_dict = {}
        self._state_dict = {}
        self._timeline_dict = {}
        self._connection_type_for_box = {}
        self._box_stack = []

    def instanciate_objects(self, topdict, root = "l0_root"):
        self._parse_box(root)
        for name, obj in self._box_dict.items():
            topdict[name] = obj
        return self._box_dict[root]

    def create_waiter_on_box(self, box, topdict):
        waiter = BehaviorWaiterClass()
        io_list = box.get_io_with_type(OutputType.STOPPED)
        for io_name in io_list:
            waiter.connectInput("onDone", box, io_name)
        self._box_dict[waiter.getName()] = waiter
        topdict[waiter.getName()] = waiter
        return waiter

    def _parse_timeline(self, box_name, parent_name):
        if (box_name in self._declared_objects):
            return

        self._declared_objects.add(box_name)
        try:
            t_file = codecs.open(self._folder_name + box_name + ".xml",
                                 encoding="utf-8", mode='r')
        except IOError:
            return None
        t_file.close()

        timeline_obj = qicore.Timeline(self._broker.getALBroker())
        timeline_obj.loadFromFile(str(self._folder_name + box_name + ".xml"))
        timeline_obj.registerOnStoppedCallback(self._box_dict[parent_name].__onTimelineStopped__)
        self._box_dict[parent_name].setTimeline(timeline_obj)
        self._timeline_dict[box_name] = timeline_obj


    def _parse_state(self, state_name):
        state = StateClass(state_name)
        dom = xml.dom.minidom.parse(self._folder_name + state_name + ".xml")
        root = dom.getElementsByTagName('State')[0]

        for obj in root.getElementsByTagName("Object"):
            objname = obj.attributes["Name"].value
            self._parse_box(objname)
            state.add_box(self._box_dict[objname])

        if (state_name in self._declared_objects):
            return state
        self._declared_objects.add(state_name)

        for label in root.getElementsByTagName("Label"):
            label_name = label.attributes["Name"].value
            state.addLabel(label_name.encode("ascii", "ignore"))

        interval = root.getElementsByTagName("Interval")[0]
        interval_begin = int(interval.attributes["begin"].value)
        interval_end = int(interval.attributes["end"].value)
        state.setInterval(interval_begin, interval_end)

        for link in root.getElementsByTagName("Link"):
            input_obj = link.attributes["InputObject"].value
            output_obj = link.attributes["OutputObject"].value
            input_name = link.attributes["InputName"].value
            output_name = link.attributes["OutputName"].value

            # create objects if needed
            self._parse_box(input_obj)
            self._parse_box(output_obj)

            ctype = self._connection_type_for_box[input_obj][input_name]
            func = f_gen.generate_connection_function(self._box_dict[input_obj],
                                                      str(input_name),
                                                      self._box_dict[output_obj],
                                                      str(output_name),
                                                      ctype)
            state.add_connection(func)


        return state

    def _parse_statemachine(self, box_name, parent_box_name):
        if (box_name in self._declared_objects):
            return

        self._declared_objects.add(box_name)
        sm_obj = qicore.StateMachine()

        try:
            dom = xml.dom.minidom.parse(self._folder_name + box_name + ".xml")
        except IOError:
            return

        root = dom.getElementsByTagName('StateMachine')[0]
        for state in root.getElementsByTagName('State'):
            statename = state.attributes["Name"].value
            state_obj = self._parse_state(statename)
            sm_obj.addState(state_obj)
            self._state_dict[statename] = state_obj

        for tra in root.getElementsByTagName('Transition'):
            from_state = tra.attributes["From"].value
            to_state = tra.attributes["To"].value
            timeout = int(tra.attributes["TimeOut"].value)
            transition_obj = qicore.Transition(self._state_dict[to_state])
            if (timeout != -1):
                transition_obj.setTimeOut(timeout)
            self._state_dict[from_state].addTransition(transition_obj)
            self._transition_dict[from_state + "__to__" + to_state] = transition_obj

        for fstate in root.getElementsByTagName('FinalState'):
            sm_obj.setFinalState(self._state_dict[fstate.attributes["Name"].value])

        initial_state = root.getElementsByTagName('InitialState')[0]
        initial_state_name = initial_state.attributes["Name"].value
        sm_obj.setInitialState(self._state_dict[initial_state_name])

        sm_obj.setName(str(box_name))
        self._box_dict[parent_box_name].setStateMachine(sm_obj)
        sm_obj.registerNewStateCallback(self._box_dict[parent_box_name].__onNewState__)

        self._statemachine_dict[box_name] = sm_obj

    def _parse_box(self, box_name):
        if (box_name in self._declared_objects):
            return

        self._declared_objects.add(box_name)

        dom = xml.dom.minidom.parse(self._folder_name + box_name + ".xml")
        root = dom.getElementsByTagName('Box')[0]

        io_info = f_gen.IOInfo()
        connection_map = {}

        for inp in root.getElementsByTagName('Input'):
            inp_name = inp.attributes["name"].value
            inp_nature = int(inp.attributes["nature"].value)
            connection_map[inp_name] = ConnectionType.INPUT
            meth = f_gen.generate_input_method(inp_nature, inp_name)
            io_info.add_input(inp_name, inp_nature, meth)

        for out in root.getElementsByTagName('Output'):
            out_name = out.attributes["name"].value
            out_type = out.attributes["type"].value
            out_nature = int(out.attributes["nature"].value)
            connection_map[out_name] = ConnectionType.OUTPUT
            meth = f_gen.generate_output_method(out_nature, out_name)
            io_info.add_output(out_name,
                               int(out_type == IOType.BANG),
                               out_nature, meth)

        for param in root.getElementsByTagName('Parameter'):
            param_name = param.attributes["name"].value
            param_value = param.attributes["value"].value
            param_content_type = param.attributes["content_type"].value
            param_inherits = param.attributes["inherits_from_parent"].value
            connection_map[param_name] = ConnectionType.PARAMETER
            io_info.add_parameter(param_name,
                                  _convert_parameter(param_value,
                                                     int(param_content_type)),
                                  param_inherits == 1)

        for res in root.getElementsByTagName("Resource"):
            resource_type = res.attributes["type"].value
            meth = f_gen.generate_resource_method(resource_type)
            io_info.add_resource(meth)

        self._connection_type_for_box[box_name] = connection_map

        module = __import__(box_name)
        box_class = getattr(module, box_name + "_class")
        box_object = box_class(io_info)
        self._box_dict[box_name] = box_object
        box_object.setPath(self._folder_name)

        if (self._box_stack):
            parent_box = self._box_stack.pop()
            self._box_stack.append(parent_box)
            box_object.setParentBox(parent_box)
        self._box_stack.append(box_object)

        self._parse_timeline(box_name + "_timeline", box_name)
        self._parse_statemachine(box_name + "_state_machine", box_name)
        self._box_stack.pop()

