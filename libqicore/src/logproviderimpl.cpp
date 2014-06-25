/*
** Author(s):
**  - Herve Cuche <hcuche@aldebaran-robotics.com>
**  - Matthieu Nottale <mnottale@aldebaran-robotics.com>
**
** Copyright (C) 2013 Aldebaran Robotics
*/

#include <boost/lexical_cast.hpp>
#include <boost/lockfree/queue.hpp>

#include <qi/type/objectfactory.hpp>
#include <qi/type/objecttypebuilder.hpp>

#include "src/logproviderimpl.hpp"

QI_TYPE_INTERFACE(LogProvider);

#define MAX_MSGS_BUFFERS 128

qiLogCategory("log.provider");

static bool logDebug = getenv("LOG_DEBUG");
#define DEBUG(a)                                \
  do {                                          \
    if (logDebug) std::cerr << a << std::endl;  \
  } while(0)

namespace qi
{
  boost::lockfree::queue<qi::LogMessage*> _pendingMessages(MAX_MSGS_BUFFERS);

  static LogProviderPtr instance;
  qi::Future<int> registerToLogger(LogManagerPtr logger)
  {
    DEBUG("registering new provider");
    if (instance)
    {
      qiLogError("Provider already registered for this process");
      return qi::Future<int>(-1);
    }

    LogProviderPtr ptr;
    try
    {
      ptr = qi::createObject("LogProvider", logger);
    }
    catch (const std::exception& e)
    {
      qiLogError() << e.what();
    }

    instance = ptr;
    DEBUG("LP registerToLogger " << &ptr);

    return logger.async<int>("addProvider", ptr);
  }

  static void silenceQiCategories(qi::log::SubscriberId subscriber)
  {
    // Safety: avoid infinite loop
    ::qi::log::setCategory("qitype.*", qi::LogLevel_Silent, subscriber);
    ::qi::log::setCategory("qimessaging.*", qi::LogLevel_Silent, subscriber);
    ::qi::log::setCategory("qi.*", qi::LogLevel_Silent, subscriber);
  }

  LogProviderImpl::LogProviderImpl(LogManagerPtr logger)
    : _logger(logger)
  {
    DEBUG("LP subscribed this " << this);
    _subscriber = qi::log::addLogHandler("remoteLogger",
                                         boost::bind(&LogProviderImpl::log,
                                                     this,
                                                     _1, _2, _3, _4, _5, _6, _7));

    DEBUG("LP subscribed " << _subscriber);
    silenceQiCategories(_subscriber);
    ++_ready;
    sendTask.setName("LogProvider");
    sendTask.setUsPeriod(100 * 1000); // 100ms
    sendTask.setCallback(&LogProviderImpl::sendLogs, this);
    sendTask.start();
  }


  LogProviderImpl::~LogProviderImpl()
  {
    DEBUG("LP ~LogProviderImpl");
    sendTask.stop();
    sendLogs();
    qi::log::removeLogHandler("remoteLogger");
  }

  void LogProviderImpl::sendLogs()
  {
    if (!_pendingMessages.empty() && _logger)
    {
      DEBUG("LP sendLogs");
      std::vector<qi::LogMessage> msgs;
      qi::LogMessage* msg;
      while (_pendingMessages.pop(msg))
      {
        msgs.push_back(*msg);
        delete msg;
      }
      try
      {
        _logger->log(msgs);
      }
      catch (const std::exception& e)
      {
        DEBUG(e.what());
      }
    }

  }

  void LogProviderImpl::log(qi::LogLevel level,
                            qi::os::timeval tv,
                            const char* category,
                            const char* message,
                            const char* file,
                            const char* function,
                            int line)
  {
    DEBUG("LP log callback: " <<  message << " " << file <<  " " << function);
    if (!*_ready)
      return;

    LogMessage* msg = new LogMessage();
    std::string source(file);
    source += ':';
    source += function;
    source += ':';
    source += boost::lexical_cast<std::string>(line);
    msg->source = source;
    msg->level = level;
    msg->timestamp = tv;
    msg->category = category;
    msg->location = qi::os::getMachineId() + ":" + boost::lexical_cast<std::string>(qi::os::getpid());
    msg->message = message;
    msg->id = -1;

    _pendingMessages.push(msg);

    DEBUG("LP:log done");
  }

  void LogProviderImpl::setLevel(qi::LogLevel level)
  {
    DEBUG("LP verb " << level);
    ::qi::log::setVerbosity(level, _subscriber);
  }

  void LogProviderImpl::addFilter(const std::string& filter,
                                  qi::LogLevel level)
  {
    DEBUG("LP addFilter level: " << level << " cat: " << filter);
    {
      boost::mutex::scoped_lock sl(_setCategoriesMutex);
      _setCategories.insert(filter);
    }
    ::qi::log::setCategory(filter, level, _subscriber);
  }

  void LogProviderImpl::setFilters(const std::vector<std::pair<std::string, qi::LogLevel> >& filters)
  {
    DEBUG("LP setFilters");
    {
      boost::mutex::scoped_lock sl(_setCategoriesMutex);
      for (std::set<std::string>::iterator it = _setCategories.begin(); it != _setCategories.end(); ++it)
      {
        if (*it != "*")
          ::qi::log::setCategory(*it, qi::LogLevel_Debug, _subscriber);
      }

      _setCategories.clear();
    }
    qi::LogLevel wildcardLevel = qi::LogLevel_Silent;
    bool wildcardIsSet = false;
    for (unsigned i = 0; i < filters.size(); ++i)
    {
      if (filters[i].first == "*")
      {
        wildcardLevel = filters[i].second;
        wildcardIsSet = true;
      }
      else
        addFilter(filters[i].first, filters[i].second);
    }

    silenceQiCategories(_subscriber);

    if (wildcardIsSet)
      ::qi::log::setCategory("*", wildcardLevel, _subscriber);
  }

  QI_REGISTER_MT_OBJECT(LogProvider, setLevel, addFilter, setFilters);
  QI_REGISTER_IMPLEMENTATION(LogProvider, LogProviderImpl);
  QI_REGISTER_OBJECT_FACTORY_CONSTRUCTOR_FOR(LogProvider, LogProviderImpl, LogManagerPtr);
} // !qi
