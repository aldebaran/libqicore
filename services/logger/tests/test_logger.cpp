
#include <gtest/gtest.h>

#include <qi/application.hpp>

#include <qimessaging/session.hpp>
#include <testsession/testsessionpair.hpp>

#include "src/logger.hpp"
#include "logger_proxy.hpp"
#include "loglistener_proxy.hpp"


LoggerProxyPtr logger;
LogListenerProxyPtr listener;

void startClient(qi::Session& s, const std::string& serviceName)
{
  qi::ObjectPtr glogger = s.service(serviceName);
  ASSERT_TRUE(glogger);
  logger = LoggerProxyPtr(new LoggerProxy(glogger));
  listener = logger->getListener();
  // we lose glogger, but logger keeps it alive
  // can we lose logger? In theory yes
}

std::string startService(qi::Session& s)
{
  std::vector<std::string> services = s.loadService("logger");
  EXPECT_EQ(1u, services.size());
  qi::details::printMetaObject(std::cerr, s.service(services.front()).value()->metaObject());
  return services.front();
}

qi::atomic<int> messagesCount(0);
Message* messages = new Message[100];

void onMessage(const Message& msg)
{
  std::stringstream ss;
  ss << "MESSAGE " << msg.level
    << " " << msg.source
    <<" " << msg.message
    << ' ' << msg.category
    << ' ' << msg.location
    << std::endl;
  std::cerr << ss.str() << std::endl;
  int p = ++messagesCount;
  messages[p-1] = msg;
}

bool waitMessage(int count, bool exact = true)
{
  for (int i=0; count > *messagesCount && i < 50; ++i)
    qi::os::msleep(10);
  bool ok =  exact? (count == *messagesCount): (count <= *messagesCount);
  if (!ok)
    qiLogError("test") << "Failed wait for " << count <<" : " << *messagesCount;
  return ok;
}


TEST(Logger, test)
{
  using namespace qi::log;
  TestSessionPair p;
  std::string loggerName = startService(*p.server());
  startClient(*p.client(), loggerName);
  ASSERT_TRUE(listener);
  
  // use a third session to register the provider
  qi::Session s2;
  s2.connect(p.serviceDirectoryEndpoints()[0]);
  registerToLogger(LoggerProxyPtr(new LoggerProxy(s2.service(loggerName))));
  
  listener->clearFilters();
  listener->setCategory("qi*", silent);
  listener->setCategory("qi.ThreadPool", silent);
  listener->setCategory("foo", debug);
  listener->setVerbosity(qi::log::info);
  listener->onMessage.connect(&onMessage);
  qi::os::msleep(200);
  messagesCount = 0;
  qiLogError("foo") << "bar";
  qi::os::msleep(200);
  ASSERT_TRUE(waitMessage(1, true));
  qiLogWarning("foo") << "bar";
  ASSERT_TRUE(waitMessage(2, true));
}


int main(int argc, char **argv) {
  qi::Application app(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  TestMode::initTestMode(argc, argv);
  return RUN_ALL_TESTS();
}
