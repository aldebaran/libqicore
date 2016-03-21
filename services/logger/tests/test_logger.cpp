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
  return logger->createListener();
}

int startProvider(qi::Session& s, const std::string& serviceName)
{
  qi::LogManagerPtr logger = s.service(serviceName);
  qi::LogProviderPtr ptr = qi::import("qicore").call<qi::LogProviderPtr>("makeLogProvider", logger);
  ptr->setCategoryPrefix("LogManagerTest");

  return logger->addProvider(ptr);
}

void removeProvider(qi::Session& s, const std::string& serviceName, int id)
{
  qi::LogManagerPtr logger = s.service(serviceName);
  logger->removeProvider(id);
}

std::string startService(qi::Session& s)
{
  try
  {
    s.registerService("LogManager", qi::import("logmanager").call<qi::AnyObject>("LogManager"));
  }
  catch (const std::exception& e)
  {
    std::cout << e.what() << std::endl;
  }
  qi::detail::printMetaObject(std::cerr, s.service("LogManager").value().metaObject());
  return "LogManager";
}

qi::Atomic<int> messagesCount(0);
qi::LogMessage* messages = new qi::LogMessage[100];

void onLogMessage(const qi::LogMessage& msg)
{
  std::stringstream ss;
  ss << "MESSAGE " << msg.level << " " << msg.source << " " << msg.message << " " << msg.category << " "
     << msg.location << std::endl;
  EXPECT_TRUE(msg.category.find("LogManagerTest.") != std::string::npos);
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
    ss << "MESSAGE BL" << msg.level << " " << msg.source << " " << msg.message << " " << msg.category << " "
       << msg.location << std::endl;
    std::cerr << ss.str() << std::endl;
    int p = ++messagesCount;
    messages[p - 1] = msg;
  }
}

bool waitLogMessage(int count, bool exact = true)
{
  for (int i = 0; count > messagesCount.load() && i < 50; ++i)
    qi::os::msleep(10);

  bool ok = exact ? (count == messagesCount.load()) : (count <= messagesCount.load());
  if (!ok)
    std::cerr << "ERROR: Failed wait for " << count << " : " << messagesCount.load() << std::endl;

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
  qi::os::msleep(100);
  messagesCount = 0;
  qiLogError("foo") << "bar";
  ASSERT_TRUE(waitLogMessage(1, true));
  qiLogWarning("foo") << "bar";
  ASSERT_TRUE(waitLogMessage(2, true));

  qi::LogManagerPtr logger = (*p.client()).service(loggerName);
  logger->removeProvider(id);
  listener.reset();
}

TEST(Logger, TestWithoutService)
{
  using namespace qi::log;
  TestSessionPair p;
  qi::LogProviderPtr ptr = qi::import("qicore").call<qi::LogProviderPtr>("makeLogProvider");

  messagesCount = 0;
  qiLogError("LogManagerTest.foo") << "barBL";
  qiLogWarning("LogManagerTest.foo") << "barBL";
  qiLogInfo("LogManagerTest.foo") << "barBL";

  std::string loggerName = startService(*p.server());

  qi::LogListenerPtr listener = startClient(*p.client(), loggerName);
  ASSERT_TRUE(listener);

  listener->clearFilters();
  listener->addFilter("foo", qi::LogLevel_Debug);
  listener->setLevel(qi::LogLevel_Info);
  listener->onLogMessage.connect(&onLogMessage);

  qi::LogManagerPtr logger = (*p.client()).service(loggerName);
  ptr->setLogger(logger);
  int id = logger->addProvider(ptr);

  ASSERT_TRUE(waitLogMessage(3, true));

  qiLogError("LogManagerTest.foo") << "bar";
  ASSERT_TRUE(waitLogMessage(4, true));
  qiLogWarning("LogManagerTest.foo") << "bar";
  ASSERT_TRUE(waitLogMessage(5, true));

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
  qiLogError("foo") << "barBL";
  qiLogWarning("foo") << "barBL";
  qiLogInfo("foo") << "barBL";
  qi::LogListenerPtr listener = startClient(*p.client(), loggerName);
  ASSERT_TRUE(listener);

  listener->clearFilters();
  listener->addFilter("foo", qi::LogLevel_Debug);
  listener->setLevel(qi::LogLevel_Info);
  listener->onLogMessagesWithBacklog.connect(&onLogMessages);
  ASSERT_TRUE(waitLogMessage(3, true));

  qiLogError("foo") << "bar";
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
  qi::os::msleep(100);
  messagesCount = 0;
  qiLogError("foo") << "bar";
  ASSERT_TRUE(waitLogMessage(1, true));
  qiLogWarning("foo") << "bar";
  ASSERT_TRUE(waitLogMessage(2, true));

  qi::LogManagerPtr logger = (*p.client()).service(loggerName);
  logger->removeProvider(id);

  qiLogError("foo") << "bar";
  qi::os::msleep(300); // if message isn't arrive yet it's probably it will never arrive
  ASSERT_EQ(messagesCount.load(), 2);
  qiLogWarning("foo") << "bar";
  qi::os::msleep(300);
  ASSERT_EQ(messagesCount.load(), 2);

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
  qi::os::msleep(100);
  messagesCount = 0;
  qiLogError("foo") << "bar";
  qi::os::msleep(300); // if message isn't arrive yet it's probably it will never arrive
  ASSERT_EQ(messagesCount.load(), 0);
  qiLogWarning("foo") << "bar";
  qi::os::msleep(300);
  ASSERT_EQ(messagesCount.load(), 0);

  qi::LogManagerPtr logger = (*p.client()).service(loggerName);
  logger->removeProvider(id);
  listener.reset();
}

struct OldLogMessage
{
  std::string source;                     // File:function:line
  qi::LogLevel level;                     // Level of verbosity of the message
  qi::os::timeval timestamp;              // deprecated: timestamp when the message have been posted
  std::string category;                   // Category of the message
  std::string location;                   // machineID:PID
  std::string message;                    // The message itself
  unsigned int id;                        // Unique message ID
};
QI_TYPE_STRUCT(OldLogMessage, source, level, timestamp, category, location, message, id);

TEST(Logger, FillMessageFields)
{
  OldLogMessage oMsg;
  oMsg.source = "toto.cpp";
  oMsg.level = qi::LogLevel_Fatal;
  oMsg.category = "dafuq";
  oMsg.location = "testFunc:11";
  oMsg.message = "YOLOOOOO";
  oMsg.id = 10;
  oMsg.timestamp = qi::os::timeval(1, 99);

  qi::LogMessage msg;
  EXPECT_NO_THROW(msg = qi::AnyValue::from(oMsg).to<qi::LogMessage>());
  EXPECT_EQ(qi::SystemClock::time_point(qi::Seconds(1) + qi::MicroSeconds(99)), msg.systemDate);
  EXPECT_EQ(qi::Clock::time_point(), msg.date);
}

TEST(Logger, DropMessageFields)
{
  qi::LogMessage msg;
  msg.source = "toto.cpp";
  msg.level = qi::LogLevel_Fatal;
  msg.category = "dafuq";
  msg.location = "testFunc:11";
  msg.message = "YOLOOOOO";
  msg.id = 10;
  msg.date = qi::Clock::time_point(qi::Seconds(2) + qi::MicroSeconds(99));
  msg.systemDate = qi::SystemClock::time_point(qi::Seconds(1) + qi::MicroSeconds(99));

  OldLogMessage oMsg;
  EXPECT_NO_THROW(oMsg = qi::AnyValue::from(msg).to<OldLogMessage>());
  qi::os::timeval tm = oMsg.timestamp;
  EXPECT_EQ(1, tm.tv_sec);
  EXPECT_EQ(99, tm.tv_usec);
}

int main(int argc, char** argv)
{
  qi::Application app(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  TestMode::initTestMode(argc, argv);
  return RUN_ALL_TESTS();
}
