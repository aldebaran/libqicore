/*
** Author(s):
**  - Herve Cuche <hcuche@aldebaran-robotics.com>
**  - Matthieu Nottale <mnottale@aldebaran-robotics.com>
**
** Copyright (C) 2013 Aldebaran Robotics
*/

#include <qi/session.hpp>
#include <boost/thread.hpp>

#include <qi/application.hpp>
#include <qi/anymodule.hpp>

#include <qicore/logmessage.hpp>
#include <qicore/logprovider.hpp>
#include <qicore/logmanager.hpp>
#include <qicore/loglistener.hpp>

void ping()
{
  while (true)
  {
    qiLogInfo("test.dump") << "ping";
    qi::os::msleep(900);
  }
}

void onLogMessage(int i)
{
  std::cerr << "MESSAGE " << i << std::endl;
}

void onLogMessage2(const qi::LogMessage& msg)
{
  std::cerr << "MESSAGE " << msg.level
            << " " << msg.source
            << " " << msg.message
            << ' ' << msg.category
            << ' ' << msg.location
            << std::endl;
}

int main(int argc, char** argv)
{
  qi::Application app(argc, argv);
  qi::Session s;
  std::string target = "tcp://localhost:9559";
  if (argc > 1)
    target = argv[1];

  s.connect(target);
  qi::LogManagerPtr logger = s.service("LogManager");
  assert(logger);
  qi::LogListenerPtr listener = logger->getListener();
  assert(listener);
  listener->onLogMessage.connect(&onLogMessage2);
  listener->setLevel(::qi::LogLevel_Info);
  //listener->asObject()->connect("onLogMessage", &onLogMessage);
  qi::import("qicore").call<void>("registerToLogger", logger);

  boost::thread t(&ping);
  app.run();
}
