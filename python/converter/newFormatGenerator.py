#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import os

import node
import box
import timeline
import diagram
import behaviorLayer
import behaviorKeyFrame

def write_box_meta(f, node):
  f.write("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" + os.linesep)
  f.write("<Box id=\"{}\" content=\"{}\" robot=\"{}\" tooltip=\"{}\" bitmap=\"{}\" bitmap_expanded=\"{}\" plugin=\"{}\" x=\"{}\" y=\"{}\" >{}"
            .format(node.id_,
                    node.name + ".py",
                    node.robot,
                    node.tooltip,
                    node.bitmap.replace(" ", "").replace(os.linesep, ""),
                    node.bitmap_expanded,
                    node.plugin,
                    node.x,
                    node.y,
                    os.linesep))
  for input in node.inputs:
    f.write("\t<Input name=\"{}\" type=\"{}\" type_size=\"{}\" nature=\"{}\" inner=\"{}\" tooltip=\"{}\" id=\"{}\" />{}"
        .format(input.name,
                input.type,
                input.type_size,
                input.nature,
                input.inner,
                input.tooltip,
                input.id,
                os.linesep))
  for output in node.outputs:
    f.write("\t<Output name=\"{}\" type=\"{}\" type_size=\"{}\" nature=\"{}\" inner=\"{}\" tooltip=\"{}\" id=\"{}\" />{}"
        .format(output.name,
                output.type,
                output.type_size,
                output.nature,
                output.inner,
                output.tooltip,
                output.id,
                os.linesep))
  for parameter in node.parameters:
    f.write("\t<Parameter name=\"{}\" inherits_from_parent=\"{}\" content_type=\"{}\" value=\"{}\" default_value=\"{}\" min=\"{}\" max=\"{}\" tooltip=\"{}\" id=\"{}\" custom_choice=\"{}\" />{}"
        .format(parameter.name,
                parameter.inherits_from_parent,
                parameter.content_type,
                parameter.value,
                parameter.default_value,
                parameter.min,
                parameter.max,
                parameter.tooltip,
                parameter.id,
                parameter.custom_choice,
                os.linesep))
  #if (node.child != None):
  #  f.write("<Child name=\"{}\" />{}".format("childName", os.linesep))

  for resource in node.resources:
    f.write("\t<Resource name=\"{}\" type=\"{}\" timeout=\"{}\" />{}"
            .format(resource.name,
                    resource.type,
                    resource.timeout,
                    os.linesep))
  f.write("</Box>" + os.linesep)

def find_port_name(box, portId):
  for port in box.inputs + box.outputs + box.parameters:
    if (port.id == portId):
      return port.name
  return ""

def write_diagram_meta(f, node, idMap, boxes):
  f.write("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" + os.linesep)
  f.write("<Diagram scale=\"{}\" >{}".format(node.scale, os.linesep))

  for num, name in idMap.items():
    if (num != "0"):
      f.write("\t<Object Name=\"{}\" />{}".format(name, os.linesep))

  for link in node.links:
    f.write("\t<Link InputObject=\"{}\" InputName=\"{}\" OutputObject=\"{}\" OutputName=\"{}\" />{}"
        .format(idMap[link.inputowner],
                find_port_name(boxes[idMap[link.inputowner]], link.indexofinput),
                idMap[link.outputowner],
                find_port_name(boxes[idMap[link.outputowner]], link.indexofoutput),
                os.linesep))
  f.write("</Diagram>" + os.linesep)

def write_actuatorList(f, node):
  f.write("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" + os.linesep)
  f.write("<Timeline fps=\"{}\" resources_acquisition=\"{}\" size=\"{}\" enable=\"{}\" start_frame=\"{}\" end_frame=\"{}\" scale=\"{}\" >{}"
          .format(node.fps,
                  node.resources_acquisition,
                  node.size,
                  node.enable,
                  node.start_frame,
                  node.end_frame,
                  node.scale,
                  os.linesep))
  for actuator in node.actuatorList:
    f.write("\t<ActuatorCurve name=\"{}\" actuator=\"{}\" recordable=\"{}\" mute=\"{}\" alwaysVisible=\"{}\" >{}"
        .format(actuator.name,
                actuator.actuator,
                actuator.recordable,
                actuator.mute,
                actuator.alwaysVisible,
                os.linesep))
    for key in actuator.keys:
      f.write("\t\t<Key frame=\"{}\" value=\"{}\" />{}"
          .format(key.frame,
                  key.value,
                  os.linesep))
    f.write("\t</ActuatorCurve>{}".format(os.linesep))
  f.write("</Timeline>{}".format(os.linesep))

def build_objects_list(state):
  result = ""
  for obj in state.objects:
    result = result + obj.name + ";"
  return result

def write_state_machine(f, stateList):
  f.write("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" + os.linesep)
  f.write("<StateMachine>" + os.linesep)

  l = len(stateList)

  for i in range(l):
    f.write("\t<State Name=\"{}\" Objects=\"{}\" />{}"
            .format("state_" + str(i),
                    build_objects_list(stateList[i]),
                    os.linesep))
  f.write("\t<InitialState Name=\"{}\" />{}"
            .format("state_0",
                    os.linesep))
  f.write("\t<FinalState Name=\"{}\" />{}"
            .format("state_" + str(l - 1),
                    os.linesep))

  for i in range(l - 1):
    f.write("\t<Transition From=\"{}\" To=\"{}\" />{}"
            .format("state_" + str(i),
                    "state_" + str(i + 1),
                    os.linesep))
  f.write("</StateMachine>" + os.linesep)


class interval:
  def __init__(self, a, b, obj):
    self.begin = int(a)
    self.end = int(b)
    self.obj = obj

class state:
  def __init__(self):
    self.begin = -1
    self.end = -1
    self.obj_nb = 0
    self.objects = []

class newFormatGenerator:
  def __init__(self):
    self._namesStack = []
    self._boxNamesStack = []
    self._boxes = {}

  def constructName(self):
    result = ""
    for name in self._namesStack:
      result = result + name + "_"
    return result

  def build_idMap(self, node):
    idMap = {}
    for box in node.boxes:
      idMap[box.id_] = self.constructName() + box.name
    return idMap

  def visit(self, node):
    if (node == None):
      return
    methname = "visit_%s" % node.nodeName
    method = getattr(self, methname, self.visit)
    return method(node)

  def visit_Timeline(self, node):
    if (len(node.behaviorLayers) != 0):
      self.convertTimelineLayers(node)

    if (len(node.actuatorList) != 0):
      fname = self.constructName() + "timeline"
      f = open(fname + ".xml", encoding='utf-8', mode='w')
      write_actuatorList(f, node)
      f.close()

  def visit_Box(self, node):
    node.name = node.name.replace(" ", "_")
    fullName = self.constructName() + node.name
    self._boxes[fullName] = node
    fpy = open(fullName + ".py", encoding='utf-8', mode='w')
    #fpy.write('#!/usr/bin/env python')
    script = node.script.content.lstrip()
    script = script.replace("MyClass", fullName, 1)
    script = script.replace("(GeneratedClass):", ":", 1)
    fpy.write(script)
    fpy.write(os.linesep)
    fpy.close()
    fxml = open(fullName + '.xml', encoding='utf-8', mode='w')
    write_box_meta(fxml, node)
    fxml.close()
    self._boxNamesStack.append(self.constructName() + node.name)
    self._namesStack.append(node.name)

    self.visit(node.child)
    self._namesStack.pop()
    self._boxNamesStack.pop()

  def visit_Diagram(self, node):
    name = self.constructName() + "diagram"
    # Diagram has no name in legacy format so we fill it
    node.name = name
    for child in node.boxes:
      self.visit(child)
    idMap = self.build_idMap(node)
    if (len(self._boxNamesStack) > 0):
      idMap[str(0)] = self._boxNamesStack[len(self._boxNamesStack) - 1]
    f = open(name + ".xml", encoding='utf-8', mode='w')
    write_diagram_meta(f, node, idMap, self._boxes)
    f.close()

  def convertTimelineLayers(self, node):
    lastFrame = -1
    intervalList = []
    for layer in node.behaviorLayers:
      print("Layer : ", layer.name)
      self._namesStack.append(layer.name)

      for i in range(len(layer.behaviorKeyFrames)):
        print("-- KeyFrame: ", layer.behaviorKeyFrames[i].name, "start at: ",
            layer.behaviorKeyFrames[i].index)
        keyframe = layer.behaviorKeyFrames[i]
        if (i != (len(layer.behaviorKeyFrames) - 1)):
          nextframe = layer.behaviorKeyFrames[i + 1]
          intervalList.append(interval(keyframe.index, nextframe.index, keyframe.child))
        else:
          lastFrame = max(lastFrame, int(keyframe.index))
          intervalList.append(interval(keyframe.index, -1, keyframe.child))

        self._namesStack.append(keyframe.name)
        self.visit(keyframe.child)
        self._namesStack.pop()

      self._namesStack.pop()

    intervalList = sorted(intervalList, key=lambda inter: inter.begin)


    # Change max value of every interval
    # Assume that last state is one frame long... ?
    lastFrame = lastFrame + 1
    for inter in intervalList:
      if (inter.end == -1):
        inter.end = lastFrame;

    print("----------------- Intervals --------------------")
    for inter in intervalList:
      print(inter.begin, " -> ", inter.end)

    stateList = self.convertToStateMachine(intervalList, lastFrame)

    name = self.constructName() + "state_machine"
    f = open(name + ".xml", encoding='utf-8', mode='w')
    write_state_machine(f, stateList)
    f.close()

  def convertToStateMachine(self, intervalList, endFrame):
    if (len(intervalList) == 0):
      return [];

    stateList = []
    currentInterList = []
    currentInterList.append(intervalList.pop(0))

    while (len(currentInterList) > 0):
      currentStateBegin = (max(currentInterList, key=lambda inter: inter.begin)).begin
      currentStateEnd = 0
      nextStateBegin = endFrame

      # Retrieve all intervals that starts with the same x
      # Take the start of the next interval as well
      while (len(intervalList) > 0):
        nextInter = intervalList.pop(0)
        if (nextInter.begin == currentStateBegin):
          currentInterList.append(nextInter)
        else:
          intervalList.insert(0, nextInter)
          nextStateBegin = nextInter.begin
          break

      currentStateEnd = (min(currentInterList, key=lambda inter: inter.end)).end

      # Create the new state with data
      currentState = state()
      currentState.begin = currentStateBegin
      currentState.end = min(currentStateEnd, nextStateBegin)
      currentState.obj_nb = len(currentInterList)

      for inter in currentInterList:
        (currentState.objects).append(inter.obj)

      stateList.append(currentState)

      if (nextStateBegin < currentStateEnd):
        # Take new interval if needed
        currentInterList.append(intervalList.pop(0))
      else:
        # Clean intervals no more useful
        currentInterList = [x for x in currentInterList if not (x.end == currentStateEnd)]
        if ((currentStateEnd == nextStateBegin) and (len(intervalList) != 0)):
          currentInterList.append(intervalList.pop(0))
        if (len(currentInterList) == 0 and (len(intervalList) != 0)):
          currentInterList.append(intervalList.pop(0))


    print("----------------- States --------------------")
    for st in stateList:
      print("State : ", st.begin, " -> ", st.end, "with : ", st.obj_nb)
    return stateList
