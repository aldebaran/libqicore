#!/usr/bin/env python
import sys
import time
import qi
import qi.path
import qi.logging
from ctypes import *

if __name__ == "__main__":
    app = qi.ApplicationSession(sys.argv)
    app.start()

    mod = qi.module("qicore")
    logmanager = app.session.service("LogManager")

    # Create a provider
    provider = mod.createObject("LogProvider", logmanager)
    # Add the provider to LogManager
    id = logmanager.addProvider(provider)

    # log!!!!!
    mylogger = qi.Logger("myfoo.bar")
    mylogger.fatal("fatal log")
    mylogger.error("error log")
    mylogger.warning("warning log")
    mylogger.info("info log")
    mylogger.verbose("verbose log")

    # since LogManager hold a "proxy" to the client object
    # we need to explicitly destroy the remote provider to call
    # the destructor
    logmanager.removeProvider(id);
