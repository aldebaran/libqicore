#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import sys
import logging

import allog
import qimessagingswig
from converter.xar_types import InputType, OutputType

import qicore

# This class just bind a Timeline and a StateMachine
class TimelineLegacy:

    def __init__(self, timeline, state_machine):
        self._statemachine = state_machine
        self._timeline = timeline

    def goTo(self, label):
        frame = label
        if (self._statemachine is not None):
            frame = self._statemachine.goToLabel(label)
        if (self._timeline is not None):
            self._timeline.goTo(frame)

class BehaviorLogHandler(logging.Handler):

    def __init__(self):
        logging.Handler.__init__(self)

    def emit(self, record):
        level_to_function = {logging.DEBUG: allog.debug,
                             logging.INFO: allog.info,
                             logging.WARNING: allog.warning,
                             logging.ERROR: allog.error,
                             logging.CRITICAL: allog.fatal}

        function = level_to_function.get(record.levelno, allog.debug)
        function(record.getMessage(),
                 record.name,
                 record.filename,
                 record.funcName,
                 record.lineno)

class BehaviorLegacy(qicore.Box):

    def __init__(self, name, io_info = None):
        qicore.Box.__init__(self)
        self.setName(name)
        self.boxName = name # Created for compat purposes
        self.resource = False
        self._load_count = 0
        self._parent_box = None
        self.behaviorId = name

        self._input_signal_map = {}
        self._output_signal_map = {}
        self._io_type_map = {}
        self._parameter_map = {}
        self._callback_id_map = {}
        self._connection_counter = {}
        self._previous_on_load = []

        # Register default callbacks
        self.registerOnLoadCallback(self.__onLoad__)
        self.registerOnUnloadCallback(self.__onUnload__)

        self.logger = logging.getLogger(name)
        logHandler = BehaviorLogHandler()
        self.logger.addHandler(logHandler)
        self.logger.setLevel(logging.DEBUG)

        # Create IO on box
        if io_info:
            io_info.add_io_to_box(self)


    def print_debug(self, mystr):
        self.logger.debug(str(self.getName() + " : " + mystr))

    def print_warn(self, mystr):
        self.logger.warning(str(self.getName() + " : " + mystr))

    def print_error(self, mystr):
        self.logger.error(str(self.getName() + " : " + mystr))

    def print_info(self, mystr):
        self.logger.info(str(self.getName() + " : " + mystr))

    def print_fatal(self, mystr):
        sys.stderr.write("[FATAL] " + self.getName() + " : " + mystr)

    def waitResourcesCallback(self, callback_name):
        self.print_debug("waitResourcesCallback " + callback_name)
        self.print_error("Not implemented yet")

    def isResourceFree(self, resource_name_list):
        self.print_debug("isResourceFree")
        self.print_error("Not implemented yet")
        return True

    def waitResourceFree(self):
        self.print_debug("waitResourceFree")
        self.print_error("Not implemented yet")

    def waitResources(self):
        self.print_debug("waitResources")
        self.print_error("Not implemented yet")

    def releaseResource(self):
        self.print_debug("releaseResource")
        self.print_error("Not implemented yet")

    def _add_to_io_type_map(self, name, ctype):
        if ctype:
            if not ctype in self._io_type_map:
                self._io_type_map[ctype] = [name]
            else:
                self._io_type_map[ctype].append(name)

    def addInput(self, input_name, ctype = None):
        self.print_debug("addInput with name " + input_name)
        self._add_to_io_type_map(input_name, ctype)
        self._input_signal_map[input_name] = qimessagingswig.qi_signal()

    def addOutput(self, output_name, is_bang, ctype = None):
        self.print_debug("addOutput with name " + output_name + " is_bang : "
                          + str(is_bang))
        self._add_to_io_type_map(output_name, ctype)
        self._output_signal_map[output_name] = qimessagingswig.qi_signal()
        self.print_warn("IsBang parameter is not used for the moment")

    def addParameter(self, parameter_name, value, parentValue):
        self.print_debug("addParameter with name " + parameter_name)
        self._parameter_map[parameter_name] = value
        self.print_warn("parentValue parameter is not used for the moment")

    def getParameter(self, parameter_name):
        self.print_debug("getParameter with name " + parameter_name)
        if (parameter_name in self._parameter_map):
            return self._parameter_map[parameter_name]
        else:
            self.print_error("Parameter with name : " + parameter_name
                              + " is unknown")
            return None

    def getParametersList(self):
        self.print_debug("getParametersList")
        p_list = []
        for name, param in self._parameter_map.items():
            p_list.append(name)
        return p_list

    def setParameter(self, parameter_name, value):
        self.print_debug("setParameter with name " + parameter_name)
        if (parameter_name in self._parameter_map):
            self._parameter_map[parameter_name] = value
        else:
            self.print_error("Parameter with name : " + parameter_name
                              + " is unknown")

    def stimulateIO(self, name, param):
        self.print_debug("stimulateIO " + name)
        if (name in self._input_signal_map):
            self._input_signal_map[name].trigger(param)
        elif (name in self._output_signal_map):
            self._output_signal_map[name].trigger(param)
        else:
            self.print_warn("Unable to stimulate IO " + name
                            + " : signal not found")

    def BIND_PYTHON(self, module_name, method_name, param_count = -1):
        self.print_debug("BIND_PYTHON " + method_name + " from " + module_name
                    + " with " + str(param_count))
        self.print_warn("This function is no more used")

    def _findUserMethod(self, method_name):
        if (method_name in dir(self)):
            return getattr(self, method_name)
        else:
            return None

    def addCallbackToSignal(self, signal_name, callback):
        if (signal_name in self._input_signal_map):
            return self._input_signal_map[signal_name].connect(callback)
        elif (signal_name in self._output_signal_map):
            return self._output_signal_map[signal_name].connect(callback)
        else:
            return False

    def removeCallbackToSignal(self, signal_name, callback_id):
        if (signal_name in self._input_signal_map):
            return self._input_signal_map[signal_name].disconnect(callback_id)
        elif (signal_name in self._output_signal_map):
            return self._output_signal_map[signal_name].disconnect(callback_id)
        else:
            return False

    def _connect(self, target, connection_name, signal_name, callback):
        if (connection_name in self._connection_counter):
            self.print_debug("Already connected: increment connection counter")
            self._connection_counter[connection_name] = self._connection_counter[connection_name] + 1
        else:
            self.print_debug("Counter == zero : Connect")
            signal_id = target.addCallbackToSignal(signal_name, callback)
            if signal_id:
                self._callback_id_map[connection_name] = signal_id
                self._connection_counter[connection_name] = 1
            else:
                self.print_error("An error occured during connection... ")

    def connectInput(self, input_name, target, signal_name):
        self.print_debug("ConnectInput " + input_name + " from " + self.getName()
                + " to " + signal_name + " from " + target.getName())

        method_name = "onInput_" + input_name + "__"
        connection_name = str(method_name + target.getName() + signal_name)
        callback = self._findUserMethod(method_name)
        if not callback:
            self.print_error("Unable to find callback method to connect")
            return
        self._connect(target, connection_name, signal_name, callback)

    def connectOutput(self, output_name, target, signal_name):
        self.print_debug("ConnectOutput " + output_name + " from "
                         + self.getName() + " to " + signal_name
                         + " from " + target.getName())

        method_name = output_name
        connection_name = str(method_name + target.getName() + signal_name)
        callback = self._findUserMethod(method_name)
        if not callback:
            self.print_error("Unable to find callback method to connect")
            return
        self._connect(target, connection_name, signal_name, callback)

    def _generate_parameter_callback(self, parameter_name, parameterMap):
        def callback(value):
            if (not parameter_name in parameterMap):
                return
            else:
                parameterMap[parameter_name] = value
        return callback

    def connectParameter(self, parameter_name, target, signal_name):
        self.print_debug("ConnectParameter " + parameter_name
                          + " from " + self.getName()
                          + " to " + signal_name + " from " + target.getName())

        connection_name = str(parameter_name + target.getName() + signal_name)
        callback = self._generate_parameter_callback(parameter_name,
                                                     self._parameter_map)
        if not callback:
            self.print_error("Unable to find callback method to connect")
            return
        self._connect(target, connection_name, signal_name, callback)

    def _disconnect(self, target, signal_name, connection_name):
        if (not (connection_name in self._connection_counter)):
            self.print_error("Unable to disconnect, this connection is unknown")
            return

        self._connection_counter[connection_name] = self._connection_counter[connection_name] - 1
        if (self._connection_counter[connection_name] == 0):
            self.print_debug("Counter == zero : Disconnect")

            ret = target.removeCallbackToSignal(signal_name, self._callback_id_map[connection_name])
            del self._connection_counter[connection_name]
            del self._callback_id_map[connection_name]

    def disconnectInput(self, input_name, target, signal_name):
        self.print_debug("disconnectInput " + input_name + " from "
                        + self.getName() + " to " + signal_name
                        + " from " + target.getName())

        method_name = "onInput_" + input_name + "__"
        connection_name = str(method_name + target.getName() + signal_name)
        self._disconnect(target, signal_name, connection_name)

    def disconnectOutput(self, output_name, target, signal_name):
        self.print_debug("disconnectOutput " + output_name + " from "
                        + self.getName() + " to " + signal_name
                        + " from " + target.getName())

        connection_name = str(output_name + target.getName() + signal_name)
        self._disconnect(target, signal_name, connection_name)

    def disconnectParameter(self, parameter_name, target, signal_name):
        self.print_debug("disconnectParameter " + parameter_name + " from "
                        + self.getName() + " to " + signal_name
                        + " from " + target.getName())

        connection_name = str(parameter_name + target.getName() + signal_name)
        self._disconnect(target, signal_name, connection_name)

    def __onLoad__(self):
        # Load the box only once

        self._load_count = self._load_count + 1

        if (self._load_count == 1):
            self.print_debug("Load")
            self._safeCallOfUserMethod("onLoad", None)

    def __onUnload__(self):
        if (self._load_count > 0):
            self._load_count = self._load_count - 1

        if (self._load_count == 0):
            if (self.resource):
                self.releaseResource()
            self.print_debug("Unload")
            self._safeCallOfUserMethod("onUnload", None)
            self._previous_on_load = []

    def _safeCallOfUserMethod(self, function_name, function_arg):
        try:
            if(function_name in dir(self)):
                func = getattr(self, function_name)
                if(func.im_func.func_code.co_argcount == 2):
                    func(function_arg)
                else:
                    func()
            return True
        except BaseException, err:
            self.print_fatal("Call to : " + function_name
                              + " failed with : " + str(err))
            try:
                if("onError" in dir(self)):
                    self.onError(self.getName() + ':' +str(err))
            except BaseException, err2:
                self.print_error(str(err2))
        sys.exit(2)
        return False

    def log(self, string):
        self.print_info(string)

    def setParentBox(self, box):
        self._parent_box = box

    def get_io_with_type(self, ctype):
        if ctype in self._io_type_map:
            return self._io_type_map[ctype]

    # This method is called by the Timeline when the job is done
    def __onTimelineStopped__(self):
        if (self.hasStateMachine()):
            self.getStateMachine().stop()

        # Stimulate STOPPED outputs
        if OutputType.STOPPED in self._io_type_map:
            for sig_name in self._io_type_map[OutputType.STOPPED]:
                self._output_signal_map[sig_name].trigger(None)

    # This method is called by the state machine when entering a new state
    # This method will activate the right boxes
    def __onNewState__(self):
        self.stimulateIO("onLoad", None)
        self._previous_on_load.append(self._input_signal_map["onLoad"])
        self._input_signal_map["onLoad"] = qimessagingswig.qi_signal()

    # Compatibility layer for Parent Timeline Control
    def getParentTimeline(self):
        return TimelineLegacy(self._parent_box.getTimeline(),
                              self._parent_box.getStateMachine())

    def stopTimelineParent(self):
        # Yup, Stop is just a pause in legacy format
        if not self._parent_box:
            return
        if (self._parent_box.hasTimeline()):
            self._parent_box.getTimeline().pause()
        if (self._parent_box.hasStateMachine()):
            self._parent_box.getStateMachine().pause()

    def playTimelineParent(self):
        if not self._parent_box:
            return
        if (self._parent_box.hasTimeline()):
            self._parent_box.getTimeline().play()
        if (self._parent_box.hasStateMachine()):
            self._parent_box.getStateMachine().run()

    def gotoParent(self, label):
        frame = label
        if (self._parent_box.hasStateMachine()):
            frame = self._parent_box.getStateMachine().goToLabel(label)
        if (self._parent_box.hasTimeline()):
            self._parent_box.getTimeline().goTo(frame)

    def gotoAndPlayParent(self, label):
        if not self._parent_box:
            return
        self.stopTimelineParent()
        self.gotoParent(label)
        self.playTimelineParent()

    def gotoAndStopParent(self, label):
        if not self._parent_box:
            return
        self.stopTimelineParent()
        self.gotoParent(label)

    # Compatibilty layer for FrameManager
    def getBehaviorPath(self, behavior_id):
        return self.getPath()

