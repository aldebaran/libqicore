/*
** Author(s):
**  - Herve Cuche <hcuche@aldebaran-robotics.com>
**  - Matthieu Nottale <mnottale@aldebaran-robotics.com>
**
** Copyright (C) 2013 Aldebaran Robotics
*/

#include <qi/os.hpp>
#include <qi/anymodule.hpp>
#include "src/loglistenerimpl.hpp"
#include "src/logmanagerimpl.hpp"

QI_TYPE_INTERFACE(LogListener);

static bool debug = getenv("LOG_DEBUG");
#define DEBUG(a)                                \
  do {                                          \
    if (debug) std::cerr << a << std::endl;     \
  } while(0)

/* We have multiple inputs: logproviders that push messages and that we
 * must configure to avoid them wasting bandwidth. They all have the same conf
 *
 * We have multiple outputs: logsubscribers on which we push messages, each
 * with its own configuration
 *
 * So we must configure all providers with the most verbose combination of
 * subscriber settings
 *
 * This is tricky with globbing.
 * a: -foo.bar  b: +foo.*  <- we must drop rule -foo.bar
 * This we can handle quite easily if we restrict globbing to terminating-star
 * a: -foo.bar b: +foo.*-foo.bar
 * Here we can keep the -foo.bar, but this is becoming tricky to track.
 * a: foo.*baz=verbose, fo*z=debug, b: f*o*z=info
 * Haha...no.
 *
 * Delegating all the work to the local logger of the service is tempting,
 * but the task is subtly different
 */

namespace qi
{
  bool set_verbosity(qi::LogListenerImpl* ll,
                     qi::LogLevel& level,
                     const qi::LogLevel& newvalue)
  {
    DEBUG("LL verbosity prop " << level);
    qi::LogLevel old = level;
    level = newvalue;
    ll->_logger.recomputeVerbosities(old, newvalue);

    return true;
  }

  // LogListenerImpl Class
  LogListenerImpl::LogListenerImpl(LogManagerImpl& l)
    :  LogListener(qi::Property<qi::LogLevel>::Getter(),
                   boost::bind(&set_verbosity, this, _1, _2))
    , _logger(l)
  {
    DEBUG("LL ctor logger: " << &_logger << " this: " << this);
    logLevel.set(qi::LogLevel_Debug);
  }

  // LogListenerImpl Class
  LogListenerImpl::LogListenerImpl(LogManagerImpl& l,
                                   boost::function<void (qi::LogListener*)> func)
    :  LogListener(qi::Property<qi::LogLevel>::Getter(),
                   boost::bind(&set_verbosity, this, _1, _2),
                   boost::bind(func, this))
    , _logger(l)
  {
    DEBUG("LL ctor logger: " << &_logger << " this: " << this);
    logLevel.set(qi::LogLevel_Debug);
  }

  LogListenerImpl::~LogListenerImpl()
  {
    DEBUG("LL ~LogListener logger: " << &_logger <<  " this: " << this);
  }

  void LogListenerImpl::setLevel(qi::LogLevel level)
  {
    DEBUG("LL verbosity " << level << " logger: " << &_logger << " this: " << this);
    logLevel.set(level);
  }

  void LogListenerImpl::clearFilters()
  {
    DEBUG("LL clearFilters logger: " << &_logger << " this: " << this);
    {
      boost::mutex::scoped_lock filtersLock(_filtersMutex);
      _filters.clear();
    }
    _logger.recomputeCategories();
  }

  void LogListenerImpl::addFilter(const std::string& filter,
                                  qi::LogLevel level)
  {
    DEBUG("LL addFilter logger: " << &_logger << " this: " << this);
    {
      boost::mutex::scoped_lock filtersLock(_filtersMutex);
      _filters[filter] = level;
    }
    _logger.recomputeCategories();
  }

  std::map<std::string, qi::LogLevel> LogListenerImpl::filters()
  {
    DEBUG("LL filters");
    FilterMap filtersCpy;
    {
      boost::mutex::scoped_lock filtersLock(_filtersMutex);
      filtersCpy = _filters;
    }
    return filtersCpy;
  }

  void LogListenerImpl::log(const LogMessage& msg)
  {
    DEBUG("LL:log: " << msg.message);
    if (msg.level > logLevel.get())
      return;

    // Check filters.
    // map ordering will give us more generic globbing filter first
    // so we must not stop on first negative filter, but go on
    // to see if a positive filter overrides it@
    bool pass = true;
    {
      boost::mutex::scoped_lock filtersLock(_filtersMutex);

      for (FilterMap::iterator it = _filters.begin(); it != _filters.end(); ++it)
      {
        const std::string& f = it->first;
        if (f == msg.category ||
            (f.find('*') != f.npos && qi::os::fnmatch(f, msg.category)))
        {
          pass = msg.level <= it->second;
        }
      }
    }

    DEBUG("LL:log filter " << pass);
    if (pass)
    {
      onLogMessage(msg);

      std::vector<qi::LogMessage> msgs;
      msgs.push_back(msg);
      onLogMessages(msgs);
      onLogMessagesWithBacklog(msgs);
    }
    DEBUG("LL:log done");
  }

  QI_REGISTER_MT_OBJECT(LogListener, setLevel, addFilter, clearFilters,
                        onLogMessage, onLogMessages, onLogMessagesWithBacklog, logLevel);
  QI_REGISTER_IMPLEMENTATION(LogListener, LogListenerImpl);

  void registerLogListener(qi::ModuleBuilder* mb) {
    mb->advertiseFactory<LogListenerImpl, LogManagerImpl&>("LogListener");
  }

} // !qi
