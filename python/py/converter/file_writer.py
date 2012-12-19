#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import codecs
import os
import cgi

def write_box_meta(f, node):
    if (node.tooltip is None):
        node.tooltip = ""

    f.write(u"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" + os.linesep)
    f.write((u"<Box id=\"{}\" content=\"{}\" robot=\"{}\" tooltip=\"{}\""
             + u" bitmap=\"{}\" bitmap_expanded=\"{}\" plugin=\"{}\""
             + u" x=\"{}\" y=\"{}\" >{}")
            .format(node.id_,
                    node.name + ".py",
                    node.robot,
                    cgi.escape(node.tooltip, quote=True),
                    node.bitmap.replace(" ", "").replace(os.linesep, ""),
                    node.bitmap_expanded,
                    node.plugin,
                    node.x_pos,
                    node.y_pos,
                    os.linesep))

    for input in node.inputs:
        if (input.tooltip is None):
            input.tooltip = ""
        if input.stm_value_name:
            f.write((u"\t<Input name=\"{}\" type=\"{}\" type_size=\"{}\""
                 + u" nature=\"{}\" stm_value_name=\"{}\" inner=\"{}\""
                 + u" tooltip=\"{}\" id=\"{}\" />{}")
                .format(input.name,
                        input.type,
                        input.type_size,
                        input.nature,
                        input.stm_value_name,
                        input.inner,
                        cgi.escape(input.tooltip, quote=True),
                        input.id,
                        os.linesep))
        else:
            f.write((u"\t<Input name=\"{}\" type=\"{}\" type_size=\"{}\""
                 + u" nature=\"{}\" inner=\"{}\" tooltip=\"{}\" id=\"{}\" />{}")
                .format(input.name,
                        input.type,
                        input.type_size,
                        input.nature,
                        input.inner,
                        cgi.escape(input.tooltip, quote=True),
                        input.id,
                        os.linesep))

    for output in node.outputs:
        if (output.tooltip is None):
            output.tooltip = ""
        f.write((u"\t<Output name=\"{}\" type=\"{}\" type_size=\"{}\""
                 + u" nature=\"{}\" inner=\"{}\" tooltip=\"{}\" id=\"{}\" />{}")
                .format(output.name,
                        output.type,
                        output.type_size,
                        output.nature,
                        output.inner,
                        cgi.escape(output.tooltip, quote=True),
                        output.id,
                        os.linesep))

    for parameter in node.parameters:
        if (parameter.tooltip is None):
            parameter.tooltip = ""
        f.write((u"\t<Parameter name=\"{}\" inherits_from_parent=\"{}\""
                 + u" content_type=\"{}\" value=\"{}\" default_value=\"{}\""
                 + u" min=\"{}\" max=\"{}\" tooltip=\"{}\" id=\"{}\""
                 + u" custom_choice=\"{}\" />{}")
                .format(parameter.name,
                        parameter.inherits_from_parent,
                        parameter.content_type,
                        parameter.value,
                        parameter.default_value,
                        parameter.min,
                        parameter.max,
                        cgi.escape(parameter.tooltip, quote=True),
                        parameter.id,
                        parameter.custom_choice,
                        os.linesep))

    for resource in node.resources:
        f.write(u"\t<Resource name=\"{}\" type=\"{}\" timeout=\"{}\" />{}"
                        .format(resource.name,
                                resource.type,
                                resource.timeout,
                                os.linesep))

    f.write(u"</Box>" + os.linesep)

def write_state_meta(f, node):
    f.write(u"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" + os.linesep)
    f.write(u"<State>{}".format(os.linesep))

    for diag in node.objects:
        for box in diag.boxes:
            f.write(u"\t<Object Name=\"{}\" />{}"
                    .format(box.name, os.linesep))

    f.write(u"\t<Interval begin=\"{}\" end=\"{}\" />{}"
            .format(node.begin, node.end, os.linesep))

    for label in node.labels:
        f.write(u"\t<Label Name=\"{}\" />{}"
                .format(label, os.linesep))

    for diag in node.objects:
        for link in diag.links:
            f.write((u"\t<Link InputObject=\"{}\" InputName=\"{}\""
                     + u" OutputObject=\"{}\" OutputName=\"{}\" />{}")
                    .format(link.inputowner,
                            link.inputName,
                            link.outputowner,
                            link.outputName,
                            os.linesep))
    f.write(u"</State>" + os.linesep)

def write_actuatorList(f, node):
    f.write(u"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" + os.linesep)
    f.write((u"<Timeline fps=\"{}\" resources_acquisition=\"{}\" size=\"{}\""
             + u" enable=\"{}\" start_frame=\"{}\" end_frame=\"{}\""
             + u" scale=\"{}\" >{}")
            .format(node.fps,
                    node.resources_acquisition,
                    node.size,
                    node.enable,
                    node.start_frame,
                    node.end_frame,
                    node.scale,
                    os.linesep))

    for actuator in node.actuator_list:
        f.write((u"\t<ActuatorCurve name=\"{}\" actuator=\"{}\""
                 + u" recordable=\"{}\" mute=\"{}\" alwaysVisible=\"{}\" >{}")
                .format(actuator.name,
                        actuator.actuator,
                        actuator.recordable,
                        actuator.mute,
                        actuator.alwaysVisible,
                        os.linesep))

        for key in actuator.keys:
            f.write(u"\t\t<Key frame=\"{}\" value=\"{}\" />{}"
                    .format(key.frame,
                            key.value,
                            os.linesep))

        f.write(u"\t</ActuatorCurve>{}".format(os.linesep))
    f.write(u"</Timeline>{}".format(os.linesep))

def _compute_timeout(state, fps):
    if (int(fps) == 0):
        return -1
    frames = state.end - state.begin
    fpms = float(fps) / 1000
    return int(frames / fpms)

def write_state_machine(f, machine_name, state_list, fps):
    f.write(u"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" + os.linesep)
    f.write(u"<StateMachine>" + os.linesep)

    l = len(state_list)

    timeout_table = []

    for i in range(l):
        state_name = machine_name + "_state_" + str(i)
        f.write(u"\t<State Name=\"{}\" />{}"
                .format(state_name,
                        os.linesep))
        with codecs.open(state_name + ".xml",
                         encoding='utf-8', mode='w') as sfile:
            write_state_meta(sfile, state_list[i])
        timeout_table.append(_compute_timeout(state_list[i], fps))

    f.write(u"\t<InitialState Name=\"{}\" />{}"
            .format(machine_name + "_state_0", os.linesep))
    f.write(u"\t<FinalState Name=\"{}\" />{}"
            .format(machine_name + "_state_" + str(l - 1), os.linesep))

    for i in range(l - 1):
        f.write(u"\t<Transition From=\"{}\" To=\"{}\" TimeOut=\"{}\" />{}"
                .format(machine_name + "_state_" + str(i),
                        machine_name + "_state_" + str(i + 1),
                        timeout_table[i],
                        os.linesep))

    f.write(u"</StateMachine>" + os.linesep)


def write_main(fmain):
    fmain.write(u"#!/usr/bin/env python" + os.linesep
            + u"# -*- coding: utf-8 -*-" + os.linesep + os.linesep
            + u"import os" + os.linesep
            + u"import sys" + os.linesep * 2
            + u"from naoqi import *" + os.linesep
            + u"import qicore" + os.linesep
            + u"import object_factory" + os.linesep + os.linesep
            + u"if (len(sys.argv) != 3):" + os.linesep
            + u"  print(\"Usage: python2 main.py IP_Address Port\")" + os.linesep
            + u"  sys.exit(2)" + os.linesep
            + u"broker = ALBroker(\"pythonBroker\", \"0.0.0.0\", 9600, sys.argv[1], int(sys.argv[2]))" + os.linesep
            + u"ALProxy.initProxies()" + os.linesep
            + u"factory = object_factory.ObjectFactory(os.path.dirname(sys.argv[0]), broker)" + os.linesep
            + u"root = factory.instanciate_objects(globals())" + os.linesep
            + u"waiter = factory.create_waiter_on_box(root, globals())" + os.linesep
            + u"root.__onLoad__()" + os.linesep
            + u"root.onInput_onStart__(None)" + os.linesep
            + u"waiter.wait_for_completion()" + os.linesep
            + u"root.dispose_memory_watcher()" + os.linesep)

