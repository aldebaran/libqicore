#include <qimessaging/session.hpp>
#include <boost/thread.hpp>

#include <qi/application.hpp>

#include "src/logger.hpp"
#include "logger_proxy.hpp"
#include "loglistener_proxy.hpp"

void ping()
{
  while (true)
  {
    qiLogInfo("test.dump") << "ping";
    ::usleep(900000);
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
    <<" " << msg.message
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
  qi::AnyObject glogger = s.service("LoggerService");
  LoggerProxyPtr logger(new LoggerProxy(glogger));
  assert(logger);
  LogListenerProxyPtr listener = logger->getListener();
  assert(listener);
  listener->onMessage.connect(&onMessage2);
  listener->setVerbosity(::qi::log::info);
  //listener->asObject()->connect("onMessage", &onMessage);
  registerToLogger(logger);
  boost::thread t (&ping);
  app.run();
}
