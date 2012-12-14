#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

from converter.xar_types import ConnectionType, InputType
from converter.xar_types import OutputType, ResourceMode

def generate_input_method(method_type, name):

    def on_start(self, param):
        if(not self._safeCallOfUserMethod("onInput_" + name, param)):
            self.releaseResource()
        if (self.hasTimeline()):
            self.getTimeline().play()
        if (self.hasStateMachine()):
            self.getStateMachine().run()
            self.stimulateIO(name, param)

    def on_stop(self, param):
        if(not self._safeCallOfUserMethod("onInput_" + name, param)):
            self.releaseResource()
            return
        self.stimulateIO(name, param)

    input_mmap = { InputType.ONLOAD : None,
                   InputType.UNDEF : on_stop,
                   InputType.ONSTART : on_start,
                   InputType.ONSTOP : on_stop,
                   InputType.STMVALUE: None}

    return input_mmap[method_type]

def generate_output_method(method_type, name):

    def stopped(self, param = None):
        if (self.hasTimeline()):
            self.getTimeline().stop()
        if (self.hasStateMachine()):
            self.getStateMachine().stop()
        self.stimulateIO(name, param)

    def punctual(self, param = None):
        self.stimulateIO(name, param)

    output_mmap = { OutputType.STOPPED : stopped,
                    OutputType.PUNCTUAL : punctual}

    return output_mmap[method_type]

def generate_resource_method(resource_type):

    def lock(self, resource_name):
        pass

    def stop_on_demand(self, resource_name):
        bexist = True
        try:
            self.onResourceLost()
        except:
            try:
                self.onResourceLost(None)
            except:
                bexist = False
        if (self.hasTimeline()):
            self.getTimeline().stop()
        if (self.StateMachine()):
            self.getStateMachine().stop()
        self.releaseResource()
        if (not bexist):
            try:
                self.onStopped()
            except:
                try:
                    self.onStopped(None)
                except:
                    pass

    def pause_on_demand(self, resource_name):
        if (self.hasTimeline()):
            self.getTimeline().pause()
        if (self.hasStateMachine()):
            self.getStateMachine().pause()
        self.releaseResource()
        self.waitResourceFree()
        self.waitResources()
        if (self.hasTimeline()):
            self.getTimeline().play()
        if (self.hasStateMachine()):
            self.getStateMachine().play()

    def callback_on_demand(self, resource_name):
        self._safeCallOfUserMethod("onResource", resource_name)

    resource_mmap = {ResourceMode.LOCK : lock,
                     ResourceMode.STOP_ON_DEMAND: stop_on_demand,
                     ResourceMode.PAUSE_ON_DEMAND: pause_on_demand,
                     ResourceMode.CALLBACK_ON_DEMAND: callback_on_demand}

    return resource_mmap[resource_type]

def generate_connection_function(input_box, input_name, output_box,
                                 output_name, ctype):
    connect_fmap = { ConnectionType.INPUT : "connectInput",
                     ConnectionType.OUTPUT : "connectOutput",
                     ConnectionType.PARAMETER : "connectParameter"}
    disconnect_fmap = { ConnectionType.INPUT : "disconnectInput",
                        ConnectionType.OUTPUT : "disconnectOutput",
                        ConnectionType.PARAMETER : "disconnectParameter"}
    def connect(enable):
        if (enable):
            method = getattr(input_box, connect_fmap[ctype])
            method(input_name, output_box, output_name)
        else:
            method = getattr(input_box, disconnect_fmap[ctype])
            method(input_name, output_box, output_name)

    return connect

