#!/usr/bin/env python

import sys
import qi
import qi.logging

@qi.singleThreaded()
class ListenerCallback():
    def onMessage(self, mess):
        # mess is a dictionnary with all known LogMessage information.
        print mess['message'], "with id", mess['id']

if __name__ == "__main__":
    app = qi.Application()
    app.start()

    logmanager = app.session.service("LogManager")
    listener = logmanager.createListener()
    listener.addFilter("*", qi.logging.DEBUG)
    listener.addFilter("stats.*", qi.logging.SILENT)
    listener.addFilter("core.naoqi", qi.logging.SILENT)
    listener.onLogMessage.connect(ListenerCallback().onMessage)

    app.run()
