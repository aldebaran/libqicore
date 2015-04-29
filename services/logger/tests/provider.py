#!/usr/bin/env python
import sys
import time
import qi
import qi.path
import qi.logging
from ctypes import *

if __name__ == "__main__":
    app = qi.Application()
    app.start()

    mod = qi.module("qicore")
    # Create a provider
    provider = mod.initializeLogging(app.session)

    # log!!!!!
    mylogger = qi.Logger("myfoo.bar")
    mylogger.fatal("fatal log")
    mylogger.error("error log")
    mylogger.warning("warning log")
    mylogger.info("info log")
    mylogger.verbose("verbose log")

    time.sleep(2)
