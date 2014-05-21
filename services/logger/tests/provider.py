#!/usr/bin/env python
import sys
import qi
import qi.path
import qi.logging
from ctypes import *

if __name__ == "__main__":
    cdll.LoadLibrary(qi.path.findLib("logprovider"))

    app = qi.ApplicationSession(sys.argv)
    app.start()
    logmanager = app.session.service("LogManager")

    # Create a provider
    provider = qi.createObject("LogProvider", logmanager)
    # Add the provider to LogManager
    logmanager.addProvider(provider)

    # log!!!!!
    mylogger = qi.Logger("myfoo.bar")
    mylogger.fatal("my fatal log")
    mylogger.error("my error log")
    mylogger.warning("my warning log")
    mylogger.info("my info log")
    mylogger.verbose("my verbose log")
