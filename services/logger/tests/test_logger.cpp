/*
** Author(s):
**  - Herve Cuche <hcuche@aldebaran-robotics.com>
**  - Matthieu Nottale <mnottale@aldebaran-robotics.com>
**
** Copyright (C) 2013 Aldebaran Robotics
*/

#include <gtest/gtest.h>

#include <qi/application.hpp>

#include <qimessaging/session.hpp>
#include <testsession/testsessionpair.hpp>

#include <qicore/logmessage.hpp>
#include <qicore/logprovider.hpp>
#include <qicore/logmanager_proxy.hpp>
#include <qicore/loglistener_proxy.hpp>


qi::LogListenerProxyPtr listener;

void startClient(qi::Session& s, const std::string& serviceName)
{
  qi::LogManagerProxy logger(s.service(serviceName));
  listener = logger.getListener();
}

void startProvider(qi::Session& s, const std::string& serviceName)
{
  qi::registerToLogger(qi::LogManagerProxyPtr(new qi::LogManagerProxy(s.service(serviceName))));
}

std::string startService(qi::Session& s)
{
  std::vector<std::string> services = s.loadService("logmanager");
  EXPECT_EQ(1u, services.size());
  qi::details::printMetaObject(std::cerr, s.service(services.front()).value()->metaObject());
  return services.front();
}

qi::Atomic<int> messagesCount(0);
qi::LogMessage* messages = new qi::LogMessage[100];

void onLogMessage(const qi::LogMessage& msg)
{
  std::stringstream ss;
  ss << "MESSAGE " << msg.level
     << " " << msg.source
     << " " << msg.message
     << " " << msg.category
     << " " << msg.location
     << std::endl;
  std::cerr << ss.str() << std::endl;
  int p = ++messagesCount;
  messages[p - 1] = msg;
}

bool waitLogMessage(int count, bool exact = true)
{
  for (int i = 0; count > *messagesCount && i < 50; ++i)
    qi::os::msleep(10);

  bool ok =  exact ? (count == *messagesCount) : (count <= *messagesCount);
  if (!ok)
    qiLogError("test") << "Failed wait for " << count << " : " << *messagesCount;

  return ok;
}


TEST(Logger, test)
{
  using namespace qi::log;
  TestSessionPair p;
  std::string loggerName = startService(*p.server());

// qi::Session s;
// s.connect(p.serviceDirectoryEndpoints()[0]);
// startClient(s, loggerName);

  startClient(*p.client(), loggerName);
  ASSERT_TRUE(listener);

  startProvider(*p.server(), loggerName);

  listener->clearFilters();
  //listener->setCategory("qi*", qi::LogLevel_Silent);
  //listener->setCategory("qi.ThreadPool", qi::LogLevel_Silent);
  listener->setCategory("foo", qi::LogLevel_Debug);
  listener->setVerbosity(qi::LogLevel_Info);
  listener->onLogMessage.connect(&onLogMessage);
  qi::os::msleep(200);
  messagesCount = 0;
  qiLogError("foo") << "bar";
  qi::os::msleep(200);
  ASSERT_TRUE(waitLogMessage(1, true));
  qiLogWarning("foo") << "bar";
  ASSERT_TRUE(waitLogMessage(2, true));
  listener.reset();
}


int main(int argc, char** argv)
{
  qi::Application app(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  TestMode::initTestMode(argc, argv);
  return RUN_ALL_TESTS();
}
