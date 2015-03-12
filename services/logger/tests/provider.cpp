#include <iostream>

#include <qi/applicationsession.hpp>

#include <qicore/logmessage.hpp>
#include <qicore/logmanager.hpp>
#include <qicore/logprovider.hpp>

#include <qi/anymodule.hpp>

int main(int argc, char** argv)
{
  qi::ApplicationSession app(argc, argv);
  app.start();

  // import module
  qi::AnyModule mod = qi::import("qicore");

  // get service Logger
  qi::LogManagerPtr logger = app.session()->service("LogManager");

  // register Logger Provider
  qi::LogProviderPtr provider =
      mod.call<qi::LogProviderPtr>("initializeLogging", app.session());
  qiLogFatal("myfoo.bar") << "fatal log";
  qiLogError("myfoo.bar") << "error log";
  qiLogWarning("myfoo.bar") << "warning log";
  qiLogInfo("myfoo.bar") << "info log";
  qiLogVerbose("myfoo.bar") << "verbose log";
  qiLogDebug("myfoo.bar") << "debug log";

  // Make sure that logs are send
  qi::os::msleep(500);
}
