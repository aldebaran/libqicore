class MyClass(GeneratedClass):
    def __init__(self):
        GeneratedClass.__init__(self)
        self.oldfps = ALFrameManager.getTimelineFps(self.getName())
        try:
            frameNumber.increaseParent( self.getName() );
        except BaseException, err:
            # if you don't want to use some encapsulated sub box, you will in that case. no worry...
            pass

    def onLoad(self):
        #puts code for box cleanup here
        pass

    def onUnload(self):
        #puts code for box cleanup here
        ""

    def onInput_onStart(self):
        newfps = (self.oldfps * self.getParameter("FPS (%)")) / 100
        ALFrameManager.setTimelineFps(self.getName(), newfps)
        pass

    def onInput_onStop(self):
        ""
