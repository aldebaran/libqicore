# -*- coding: utf-8 -*-
class MyClass(GeneratedClass):
    def __init__(self):
        try: # disable autoBind
            GeneratedClass.__init__(self, False)
        except TypeError: # if NAOqi < 1.14
            GeneratedClass.__init__( self )
        self.tts = ALProxy("ALTextToSpeech")
        self.sentences = {
                            "Chinese" : " 你好 ",
                            "English" : " Hello ",
                            "French" : " Bonjour ",
                            "German" : " Hallo ",
                            "Italian" : " Ciao ",
                            "Japanese" : " こんにちは ",
                            "Korean" : " 안녕하세요 ",
                            "Portuguese" : " Olá ",
                            "Brazilian" : " Olá ",
                            "Spanish" : " Hola ",
                            "Arabic" : " مرحبا ",
                            "Polish" : " Cześć ",
                            "Czech" : " Ahoj ",
                            "Dutch" : " Hallo ",
                            "Danish" : " Hej ",
                            "Finnish" : " Hei ",
                            "Swedish" : " Hallå ",
                            "Russian" : " Привет ",
                            "Turkish" : " Merhaba "
        }

    def onInput_onStart(self):
        sDefaultLang = self.tts.getLanguage()
        self.onStopped(self.sentences[sDefaultLang])
