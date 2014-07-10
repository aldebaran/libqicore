#include <iostream>

#include <qi/applicationsession.hpp>

#include <qicore/logmessage.hpp>
#include <qicore/logmanager.hpp>
#include <qicore/loglistener.hpp>

// More information on logmessage document into LogManager API
void onMessage(const qi::LogMessage& msg)
{
  std::stringstream ss;
  ss << msg.category
     << " " << msg.source
     << " " << msg.message;
  std::cout << ss.str() << std::endl;
}

int main(int argc, char** argv)
{
  qi::ApplicationSession app(argc, argv);
  app.start();

  app.loadModule("qicore");
  qi::LogManagerPtr logger = app.session()->service("LogManager");
  qi::LogListenerPtr listener = logger->getListener();
  listener->clearFilters();
  listener->setLevel(qi::LogLevel_Info);
  listener->onLogMessage.connect(&onMessage);

  app.run();
}
