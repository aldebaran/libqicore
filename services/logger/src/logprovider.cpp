/*
** Author(s):
**  - Herve Cuche <hcuche@aldebaran-robotics.com>
**  - Matthieu Nottale <mnottale@aldebaran-robotics.com>
**
** Copyright (C) 2013 Aldebaran Robotics
*/

#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>

#include <logger/logprovider.hpp>

qiLogCategory("logger.provider");

static LogProviderPtr instance;

static bool loggerDebug = getenv("LOGGER_DEBUG");
#define DEBUG(a)                                        \
  do {                                                  \
    if (loggerDebug) std::cerr << a << std::endl;       \
  } while(0)

void registerToLogger(LoggerProxyPtr logger)
{
  DEBUG("registering new provider");
  if (instance)
  {
    qiLogError("Provider already registered for this process");
    return;
  }

  LogProviderPtr ptr = boost::make_shared<LogProvider>(logger);
  instance = ptr;
  logger->addProvider(ptr, qi::MetaCallType_Queued).async();
}

LogProvider::LogProvider(LoggerProxyPtr logger)
  : _logger(logger)
{
  _subscriber = qi::log::addLogHandler("remoteLogger",
                                       boost::bind(&LogProvider::log, this, _1, _2, _3, _4, _5, _6, _7));

  DEBUG("Logprovider subscribed " << _subscriber);
  // Safety: avoid infinite loop
  ::qi::log::setCategory("qimessaging.*", qi::LogLevel_Silent, _subscriber);
  ::qi::log::setCategory("qitype.*", qi::LogLevel_Silent, _subscriber);
}

LogProvider::~LogProvider()
{
  DEBUG("~LogProvider");
  qi::log::removeLogHandler("remoteLogger");
}

void LogProvider::log(qi::LogLevel level,
                      qi::os::timeval tv,
                      const char* category,
                      const char* message,
                      const char* file,
                      const char* function,
                      int line)
{
  DEBUG("logprovider log callback");

  Message msg;
  std::string source(file);
  source += ':';
  source += function;
  source += ':';
  source += boost::lexical_cast<std::string>(line);
  msg.source = source;
  msg.level = level;
  msg.timestamp = tv;
  msg.category = category;
  msg.location = qi::os::getMachineId() + ":" + boost::lexical_cast<std::string>(qi::os::getpid());
  msg.message = message;
  _logger->log(msg, qi::MetaCallType_Queued).async();

  DEBUG("LogProvider log done");
}

void LogProvider::setVerbosity(qi::LogLevel level)
{
  DEBUG("LogProvider verb " << level);
  ::qi::log::setVerbosity(level, _subscriber);
}

void LogProvider::setCategory(const std::string& cat,
                              qi::LogLevel level)
{
  _setCategories.insert(cat);
  ::qi::log::setCategory(cat, level, _subscriber);
}

void LogProvider::clearAndSet(const std::vector<std::pair<std::string, qi::LogLevel> >& data)
{
  for (std::set<std::string>::iterator it = _setCategories.begin(); it != _setCategories.end(); ++it)
  {
    ::qi::log::setCategory(*it, qi::LogLevel_Debug, _subscriber);
  }

  _setCategories.clear();
  for (unsigned i = 0; i < data.size(); ++i)
  {
    setCategory(data[i].first, data[i].second);
  }

  ::qi::log::setCategory("qimessaging.*", qi::LogLevel_Silent, _subscriber);
}
