import time

class MyClass(GeneratedClass):
    def __init__(self):
        GeneratedClass.__init__(self)
        self.player = ALProxy('ALAudioPlayer')
        self.playerStop = ALProxy('ALAudioPlayer', True) #Create another proxy as wait is blocking if audioout is remote

    def onLoad(self):
        self.bIsRunning = False
        self.ids = []

    def onUnload(self):
        for id in self.ids:
            try:
                self.playerStop.stop(id)
            except:
                pass
        while( self.bIsRunning ):
            time.sleep( 0.2 )

    def onInput_onStart(self, p):
        self.bIsRunning = True
        try:
            if (self.getParameter("Play in loop")) :
               id = self.player.post.playFileInLoop(p,self.getParameter("Volume (%)")/100.,self.getParameter("Panorama"))
            else :
               id = self.player.post.playFileFromPosition(p,self.getParameter("Begin position (s)"),self.getParameter("Volume (%)")/100.,self.getParameter("Panorama"))
            self.ids.append(id)
            self.player.wait(id, 0)
        finally:
            try:
                self.ids.remove(id)
            except:
                pass
            if( self.ids == [] ):
                self.onStopped() # activate output of the box
                self.bIsRunning = False

    def onInput_onStop(self):
        self.onUnload()
