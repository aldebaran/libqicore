#include <iostream>

#include <qimessaging/applicationsession.hpp>

#include <qicore/logmessage.hpp>
#include <qicore/logmanager.hpp>
#include <qicore/logprovider.hpp>

#include <qitype/objectfactory.hpp>

int main(int argc, char** argv)
{
  qi::ApplicationSession app(argc, argv);
  app.start();

  // get service Logger
  qi::LogManagerPtr logger = app.session()->service("LogManager");

  // register Logger Provider
  qi::LogProviderPtr provider = qi::createObject("LogProvider", logger);
  logger->addProvider(provider);
  qiLogFatal("wtf") << "fatal log";
  qiLogError("wtf") << "error log";
  qiLogWarning("wtf") << "warning log";
  qiLogInfo("wtf") << "info log";
  qiLogVerbose("wtf") << "verbose log";
  qiLogDebug("wtf") << "debug log";

}
