#include <set>
#include <string>

#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>

#include <qi/log.hpp>

#include "logger.hpp"

#include "logger_service.hpp"
#include "logger_proxy.hpp"
#include "loglistener_proxy.hpp"
#include "logprovider_bind.cpp"

qiLogCategory("logger.provider");

using namespace qi::log;

static LogProviderPtr instance;

static bool loggerDebug = getenv("LOGGER_DEBUG");
#define DEBUG(a)                          \
do {                                      \
  if (loggerDebug) std::cerr << a << std::endl; \
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
  _subscriber = addLogHandler("remoteLogger",
    boost::bind(&LogProvider::log, this, _1, _2, _3, _4, _5, _6, _7));
  DEBUG("Logprovider subscribed " << _subscriber);
  // Safety: avoid infinite loop
  ::qi::log::setCategory(_subscriber, "qimessaging.*", silent);
  ::qi::log::setCategory(_subscriber, "qitype.*", silent);
}

LogProvider::~LogProvider()
{
  DEBUG("~LogProvider");
  removeLogHandler("remoteLogger");
}


void LogProvider::log(qi::log::LogLevel level, qi::os::timeval tv, const char* file,
                      const char* function, const char* category, const char* message, int line)
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

void LogProvider::setVerbosity(qi::log::LogLevel level)
{
  DEBUG("LogProvider verb " << level);
  ::qi::log::setVerbosity(_subscriber, level);
}

void LogProvider::setCategory(const std::string& cat, qi::log::LogLevel level)
{
  _setCategories.insert(cat);
  ::qi::log::setCategory(_subscriber, cat, level);
}

void LogProvider::clearAndSet(const std::vector<std::pair<std::string, LogLevel> >& data)
{
  for (std::set<std::string>::iterator it = _setCategories.begin(); it != _setCategories.end(); ++it)
    ::qi::log::setCategory(_subscriber, *it, debug);
  _setCategories.clear();
  for (unsigned i=0; i< data.size(); ++i)
    setCategory(data[i].first, data[i].second);
  ::qi::log::setCategory(_subscriber, "qimessaging.*", silent);
}

