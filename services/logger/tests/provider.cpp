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
  qi::LogProviderPtr provider = mod.call<qi::LogProviderPtr>("makeLogProvider", logger);
  int id = logger->addProvider(provider);
  qiLogFatal("myfoo.bar") << "fatal log";
  qiLogError("myfoo.bar") << "error log";
  qiLogWarning("myfoo.bar") << "warning log";
  qiLogInfo("myfoo.bar") << "info log";
  qiLogVerbose("myfoo.bar") << "verbose log";
  qiLogDebug("myfoo.bar") << "debug log";
  // since LogManager hold a "proxy" to the client object
  // we need to explicitly destroy the remote provider to call
  // the destructor
  logger->removeProvider(id);
}
