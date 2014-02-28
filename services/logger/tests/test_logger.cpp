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

int startProvider(qi::Session& s, const std::string& serviceName)
{
  qi::LogManagerProxyPtr logger = boost::make_shared<qi::LogManagerProxy>(s.service(serviceName));
  qi::LogProviderPtr ptr = boost::make_shared<qi::LogProvider>(qi::LogManagerProxyPtr(logger));
  qi::Future<int> id = logger->addProvider(ptr, qi::MetaCallType_Queued).async();
  return id.value();
}

void removeProvider(qi::Session& s, const std::string& serviceName, int id)
{
  qi::LogManagerProxy logger(s.service(serviceName));
  logger.removeProvider(id);
}

std::string startService(qi::Session& s)
{
  std::vector<std::string> services = s.loadService("logmanager");
  EXPECT_EQ(1u, services.size());
  qi::details::printMetaObject(std::cerr, s.service(services.front()).value().metaObject());
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

TEST(Logger, Test)
{
  using namespace qi::log;
  TestSessionPair p;
  std::string loggerName = startService(*p.server());

  startClient(*p.client(), loggerName);
  ASSERT_TRUE(listener);

  startProvider(*p.server(), loggerName);

  listener->clearFilters();
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


TEST(Logger, RemoveProviderTest)
{
  using namespace qi::log;
  TestSessionPair p;
  std::string loggerName = startService(*p.server());

  startClient(*p.client(), loggerName);
  ASSERT_TRUE(listener);


  int id = startProvider(*p.server(), loggerName);

  listener->clearFilters();
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

  qi::LogManagerProxy logger((*p.client()).service(loggerName));
  logger.removeProvider(id);
  qi::os::msleep(200);

  qiLogError("foo") << "bar";
  qi::os::msleep(200); // if message isn't arrive yet it's probably it will never arrive
  ASSERT_EQ(*messagesCount, 2);
  qiLogWarning("foo") << "bar";
  qi::os::msleep(200);
  ASSERT_EQ(*messagesCount, 2);

  listener.reset();
}


TEST(Logger, KillProviderTest)
{
  using namespace qi::log;
  TestSessionPair p;
  std::string loggerName = startService(*p.server());

  startClient(*p.client(), loggerName);
  ASSERT_TRUE(listener);

  qi::Session ses;
  ses.connect(p.serviceDirectoryEndpoints()[0]);
  int id = startProvider(ses, loggerName);
  qiLogFatal("foo") << id;
  ses.close();

  listener->clearFilters();
  listener->setCategory("foo", qi::LogLevel_Debug);
  listener->setVerbosity(qi::LogLevel_Info);
  listener->onLogMessage.connect(&onLogMessage);
  qi::os::msleep(200);
  messagesCount = 0;
  qiLogError("foo") << "bar";
  qi::os::msleep(200); // if message isn't arrive yet it's probably it will never arrive
  ASSERT_EQ(*messagesCount, 0);
  qiLogWarning("foo") << "bar";
  qi::os::msleep(200);
  ASSERT_EQ(*messagesCount, 0);
  listener.reset();
}

int main(int argc, char** argv)
{
  qi::Application app(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  TestMode::initTestMode(argc, argv);
  return RUN_ALL_TESTS();
}
