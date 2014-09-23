/*
** Author(s):
**  - Herve Cuche <hcuche@aldebaran-robotics.com>
**  - Matthieu Nottale <mnottale@aldebaran-robotics.com>
**
** Copyright (C) 2013 Aldebaran Robotics
*/

#include <gtest/gtest.h>

#include <qi/application.hpp>
#include <qi/anymodule.hpp>
#include <qi/session.hpp>

#include <testsession/testsessionpair.hpp>

#include <qicore/logmessage.hpp>
#include <qicore/logprovider.hpp>
#include <qicore/logmanager.hpp>
#include <qicore/loglistener.hpp>


qi::LogListenerPtr startClient(qi::Session& s, const std::string& serviceName)
{
  qi::LogManagerPtr logger = s.service(serviceName);
  return logger->getListener();
}

int startProvider(qi::Session& s, const std::string& serviceName)
{
  qi::LogManagerPtr logger = s.service(serviceName);
  qi::LogProviderPtr ptr = qi::import("qicore").call<qi::LogProviderPtr>("makeLogProvider", logger);

  return logger->addProvider(ptr);
}

void removeProvider(qi::Session& s, const std::string& serviceName, int id)
{
  qi::LogManagerPtr logger = s.service(serviceName);
  logger->removeProvider(id);
}

std::string startService(qi::Session& s)
{
  unsigned int serviceId = 0;
  try
  {
    serviceId = s.registerService("LogManager", qi::import("logmanager").call<qi::AnyObject>("LogManager"));
  }
  catch (const std::exception& e)
  {
    std::cout << e.what() << std::endl;
  }
  qi::details::printMetaObject(std::cerr, s.service("LogManager").value().metaObject());
  return "LogManager";
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

void onLogMessages(std::vector<qi::LogMessage> msgs)
{
  for (unsigned int i = 0; i < msgs.size(); ++i)
  {
    qi::LogMessage msg = msgs.at(i);
    std::stringstream ss;
    ss << "MESSAGE BL" << msg.level
       << " " << msg.source
       << " " << msg.message
       << " " << msg.category
       << " " << msg.location
       << std::endl;
    std::cerr << ss.str() << std::endl;
    int p = ++messagesCount;
    messages[p - 1] = msg;
  }
}

bool waitLogMessage(int count, bool exact = true)
{
  for (int i = 0; count > *messagesCount && i < 50; ++i)
    qi::os::msleep(10);

  bool ok = exact ? (count == *messagesCount) : (count <= *messagesCount);
  if (!ok)
    qiLogError("test") << "Failed wait for " << count << " : " << *messagesCount;

  return ok;
}

TEST(Logger, Test)
{
  using namespace qi::log;
  TestSessionPair p;
  std::string loggerName = startService(*p.server());

  qi::LogListenerPtr listener = startClient(*p.client(), loggerName);
  ASSERT_TRUE(listener);

  int id = startProvider(*p.server(), loggerName);

  listener->clearFilters();
  listener->addFilter("foo", qi::LogLevel_Debug);
  listener->setLevel(qi::LogLevel_Info);
  listener->onLogMessage.connect(&onLogMessage);
  qi::os::msleep(600);
  messagesCount = 0;
  qiLogError("foo") << "bar";
  qi::os::msleep(600);
  ASSERT_TRUE(waitLogMessage(1, true));
  qiLogWarning("foo") << "bar";
  ASSERT_TRUE(waitLogMessage(2, true));

  qi::LogManagerPtr logger = (*p.client()).service(loggerName);
  logger->removeProvider(id);
  listener.reset();
}


TEST(Logger, TestWithBacklog)
{
  using namespace qi::log;
  TestSessionPair p;
  std::string loggerName = startService(*p.server());

  int id = startProvider(*p.server(), loggerName);
  messagesCount = 0;
  qi::os::msleep(600);
  qiLogError("foo") << "barBL";
  qiLogWarning("foo") << "barBL";
  qiLogInfo("foo") << "barBL";
  qi::LogListenerPtr listener = startClient(*p.client(), loggerName);
  ASSERT_TRUE(listener);

  listener->clearFilters();
  listener->addFilter("foo", qi::LogLevel_Debug);
  listener->setLevel(qi::LogLevel_Info);
  listener->onLogMessagesWithBacklog.connect(&onLogMessages);
  qi::os::msleep(600);
  ASSERT_TRUE(waitLogMessage(3, true));

  qiLogError("foo") << "bar";
  qi::os::msleep(600);
  ASSERT_TRUE(waitLogMessage(4, true));
  qiLogWarning("foo") << "bar";
  ASSERT_TRUE(waitLogMessage(5, true));

  qi::LogManagerPtr logger = (*p.client()).service(loggerName);
  logger->removeProvider(id);
  listener.reset();
}

TEST(Logger, RemoveProviderTest)
{
  using namespace qi::log;
  TestSessionPair p;
  std::string loggerName = startService(*p.server());

  qi::LogListenerPtr listener = startClient(*p.client(), loggerName);
  ASSERT_TRUE(listener);

  int id = startProvider(*p.server(), loggerName);

  listener->clearFilters();
  listener->addFilter("foo", qi::LogLevel_Debug);
  listener->setLevel(qi::LogLevel_Info);
  listener->onLogMessage.connect(&onLogMessage);
  qi::os::msleep(600);
  messagesCount = 0;
  qiLogError("foo") << "bar";
  qi::os::msleep(600);
  ASSERT_TRUE(waitLogMessage(1, true));
  qiLogWarning("foo") << "bar";
  ASSERT_TRUE(waitLogMessage(2, true));

  qi::LogManagerPtr logger = (*p.client()).service(loggerName);
  logger->removeProvider(id);
  qi::os::msleep(600);

  qiLogError("foo") << "bar";
  qi::os::msleep(600); // if message isn't arrive yet it's probably it will never arrive
  ASSERT_EQ(*messagesCount, 2);
  qiLogWarning("foo") << "bar";
  qi::os::msleep(600);
  ASSERT_EQ(*messagesCount, 2);

  listener.reset();
}


TEST(Logger, KillProviderTest)
{
  using namespace qi::log;
  TestSessionPair p;
  std::string loggerName = startService(*p.server());

  qi::LogListenerPtr listener = startClient(*p.client(), loggerName);
  ASSERT_TRUE(listener);

  qi::Session ses;
  ses.connect(p.serviceDirectoryEndpoints()[0]);
  int id = startProvider(ses, loggerName);
  ses.close();

  listener->clearFilters();
  listener->addFilter("foo", qi::LogLevel_Debug);
  listener->setLevel(qi::LogLevel_Info);
  listener->onLogMessage.connect(&onLogMessage);
  qi::os::msleep(600);
  messagesCount = 0;
  qiLogError("foo") << "bar";
  qi::os::msleep(600); // if message isn't arrive yet it's probably it will never arrive
  ASSERT_EQ(*messagesCount, 0);
  qiLogWarning("foo") << "bar";
  qi::os::msleep(600);
  ASSERT_EQ(*messagesCount, 0);

  qi::LogManagerPtr logger = (*p.client()).service(loggerName);
  logger->removeProvider(id);
  listener.reset();
}

int main(int argc, char** argv)
{
  qi::Application app(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  TestMode::initTestMode(argc, argv);
  return RUN_ALL_TESTS();
}
