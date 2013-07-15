class MyClass(GeneratedClass):
    def __init__(self):
        GeneratedClass.__init__(self)
    def onInput_onStart(self):
        self.onStopped(ALFrameManager.getBehaviorPath(self.behaviorId) + self.getParameter("File path"))
