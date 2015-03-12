#include <iostream>

#include <boost/thread/mutex.hpp>

#include <qi/applicationsession.hpp>

#include <qicore/logmessage.hpp>
#include <qicore/logmanager.hpp>
#include <qicore/loglistener.hpp>

// More information on logmessage document into LogManager API
static boost::mutex m;
void onMessage(const qi::LogMessage& msg)
{
  boost::mutex::scoped_lock l(m);
  std::stringstream ss;
  ss << msg.category << " " << msg.source << " " << msg.message;
  std::cout << ss.str() << std::endl;
}

int main(int argc, char** argv)
{
  qi::ApplicationSession app(argc, argv);
  app.start();

  app.loadModule("qicore");
  qi::LogManagerPtr logger = app.session()->service("LogManager");
  qi::LogListenerPtr listener = logger->createListener();
  listener->clearFilters();
  listener->setLevel(qi::LogLevel_Info);
  listener->onLogMessage.connect(&onMessage);

  app.run();
}
