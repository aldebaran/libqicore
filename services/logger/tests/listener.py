#!/usr/bin/env python

import sys
import qi
import qi.logging

def onMessage(mess):
    print mess['message'], "with id", mess['id'] # mess is a dictionnary with all known LogMessage information.

def main(argv):
    app = qi.ApplicationSession(argv)
    app.start()
    logmanager = app.session.service("LogManager")
    listener = logmanager.getListener()
    listener.addFilter("*", qi.logging.DEBUG)
    listener.addFilter("stats.*", qi.logging.SILENT)
    listener.addFilter("core.naoqi", qi.logging.SILENT)
    listener.onLogMessage.connect(onMessage)
    app.run()

if __name__ == "__main__":
    main(sys.argv)
