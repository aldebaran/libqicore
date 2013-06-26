class MyClass(GeneratedClass):
    def __init__(self):
        try: # disable autoBind
          GeneratedClass.__init__(self, False)
        except TypeError: # if NAOqi < 1.14
          GeneratedClass.__init__( self )

    def onLoad(self):
        self.bIsRunning = False

    def onUnload(self):
        self.bIsRunning = False

    def onInput_onStart(self):
        self.bIsRunning = True

    def onInput_onStop(self):
        if( self.bIsRunning ):
            self.onUnload()
            self.onStopped()
