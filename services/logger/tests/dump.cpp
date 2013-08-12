/*
** Author(s):
**  - Herve Cuche <hcuche@aldebaran-robotics.com>
**  - Matthieu Nottale <mnottale@aldebaran-robotics.com>
**
** Copyright (C) 2013 Aldebaran Robotics
*/

#include <qimessaging/session.hpp>
#include <boost/thread.hpp>

#include <qi/application.hpp>

#include <logger/logger.hpp>
#include <logger/logprovider.hpp>
#include <services/logger/loggermanager_proxy.hpp>
#include <services/logger/loglistener_proxy.hpp>

void ping()
{
  while (true)
  {
    qiLogInfo("test.dump") << "ping";
    qi::os::msleep(900);
  }
}

void onMessage(int i)
{
  std::cerr << "MESSAGE " << i << std::endl;
}

void onMessage2(const Message& msg)
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
  qi::AnyObject glogger = s.service("LoggerManager");
  LoggerManagerProxyPtr logger(new LoggerManagerProxy(glogger));
  assert(logger);
  LogListenerProxyPtr listener = logger->getListener();
  assert(listener);
  listener->onMessage.connect(&onMessage2);
  listener->setVerbosity(::qi::LogLevel_Info);
  //listener->asObject()->connect("onMessage", &onMessage);
  registerToLogger(logger);
  boost::thread t(&ping);
  app.run();
}
