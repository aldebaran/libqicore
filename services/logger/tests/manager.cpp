/*
** Author(s):
**  - Herve Cuche <hcuche@aldebaran-robotics.com>
**  - Matthieu Nottale <mnottale@aldebaran-robotics.com>
**
** Copyright (C) 2013 Aldebaran Robotics
*/

#include <testsession/testsessionpair.hpp>

#include <qi/application.hpp>

#include <qicore/logmessage.hpp>
#include <qicore/logprovider.hpp>
#include <qicore/logmanager.hpp>
#include <qicore/loglistener.hpp>

#include <qi/anymodule.hpp>

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

std::string startService(qi::SessionPtr s)
{
  unsigned int serviceId = 0;
  try
  {
//    qi::os::dlopen("logmanager");
    serviceId = s->registerService("LogManager", qi::import("logmanager").call<qi::AnyObject>("LogManager"));
  }
  catch (const std::exception& e)
  {
    std::cout << e.what() << std::endl;
  }
  qi::details::printMetaObject(std::cerr, s->service("LogManager").value().metaObject());
  return "LogManager";
}

int main(int argc, char** argv)
{
  qi::Application app(argc, argv);
  qi::SessionPtr s = qi::makeSession();
  s->listenStandalone("tcp://127.0.0.1:6666");

  std::string loggerName = startService(s);
  qi::LogManagerPtr logger = s->service(loggerName);
  assert(logger);

  app.run();
}
