class MyClass(GeneratedClass):
  def __init__(self):
    GeneratedClass.__init__(self);
    self.bMustStop = False;
    self.bIsRunning = False;

  def onUnload(self):
    import time
    self.onInput_onStop(); # will stop current loop execution
    while self.bIsRunning:
      time.sleep(0.1)

  def onInput_onStart(self):
    #self.log( self.getName() + ": start - begin" );
    
    if( self.bIsRunning ):
      #print( self.getName() + ": already started => nothing" );
      return;

    self.bIsRunning = True;
    self.bMustStop = False;
    
    rDuration = 1.0;
    ALLeds.post.fadeRGB( "FaceLedsTop", 0xff8500, rDuration );
    ALLeds.post.fadeRGB( "FaceLedsInternal", 0xff8500, rDuration );
    ALLeds.post.fadeRGB( "FaceLedsBottom", 0xff8500, rDuration );
    ALLeds.fadeRGB( "FaceLedsExternal", 0x000000, rDuration );
    
    while( not self.bMustStop ):
      rTime = 0.5;
      ALLeds.post.fadeRGB( "FaceLedsTop", 0xff0000, rTime );
      ALLeds.post.fadeRGB( "FaceLedInternal", 0xff0000, rTime );
      ALLeds.fadeRGB( "FaceLedsBottom", 0xff0000, rTime );
      if( self.bMustStop ):
        break;       
      ALLeds.post.fadeRGB( "FaceLedsInternal", 0xff8500, rTime );
      ALLeds.fadeRGB( "FaceLedsBottom", 0xff8500, rTime );

    # end while
    self.bIsRunning = False;
    self.onStopped();

  def onInput_onStop(self):
    self.bMustStop = True; # will stop current loop execution
