/*
** Author(s):
**  - Herve Cuche <hcuche@aldebaran-robotics.com>
**  - Matthieu Nottale <mnottale@aldebaran-robotics.com>
**
** Copyright (C) 2013 Aldebaran Robotics
*/

#include <boost/make_shared.hpp>

#include <qicore/logmessage.hpp>
#include "src/logmanager.hpp"

#include <qi/os.hpp>

#include <qicore/logprovider_proxy.hpp>

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
bool set_verbosity(qi::LogListener* ll,
                   qi::LogLevel& level,
                   const qi::LogLevel& newvalue)
{
  DEBUG("LL verbosity prop " << level);
  qi::LogLevel old = level;
  level = newvalue;
  ll->_logger.recomputeVerbosities(old, newvalue);

  return true;
}

  // LogListener Class
  LogListener::LogListener(LogManager& l)
    : _logger(l)
    , verbosity(qi::Property<qi::LogLevel>::Getter(),
                boost::bind(&set_verbosity, this, _1, _2))
  {
    DEBUG("LL ctor logger: " << &_logger <<  " this: " << this);
    verbosity.set(qi::LogLevel_Debug);
    onLogMessage.setCallType(qi::MetaCallType_Queued);
  }

  LogListener::~LogListener()
  {
    DEBUG("LL ~LogListener logger: " << &_logger <<  " this: " << this);
  }

  void LogListener::setVerbosity(qi::LogLevel level)
  {
    DEBUG("LL verbosity " << level << " logger: " << &_logger << " this: " << this);
    verbosity.set(level);
  }

  void LogListener::clearFilters()
  {
    DEBUG("LL clearFilters logger: " << &_logger << " this: " << this);
    _filters.clear();
    _logger.recomputeCategories();
  }

  void LogListener::setCategory(const std::string& cat,
                                qi::LogLevel level)
  {
    DEBUG("LL setCategory logger: " << &_logger << " this: " << this);
    _filters[cat] = level;
    _logger.recomputeCategories();
  }

  void LogListener::log(const LogMessage& msg)
  {
    DEBUG("LL:log: " << msg.message);
    if (msg.level > verbosity.get())
      return;

    // Check filters.
    // map ordering will give us more generic globbing filter first
    // so we must not stop on first negative filter, but go on
    // to see if a positive filter overrides it@
    bool pass = true;
    for (FilterMap::iterator it = _filters.begin(); it != _filters.end(); ++it)
    {
      const std::string& f = it->first;
      if (f == msg.category ||
          (f.find('*') != f.npos && qi::os::fnmatch(f, msg.category)))
      {
        pass = msg.level <= it->second;
      }
    }

    DEBUG("LL:log filter " << pass);
    if (pass)
      onLogMessage(msg);
  }


  LogManagerPtr make_LogPtr()
  {
    return boost::make_shared<LogManager>();
  }

  // LogManager Class
  LogManager::LogManager()
    : _maxLevel(qi::LogLevel_Silent)
  {
    DEBUG("LM instanciating");
  }

  LogManager::~LogManager()
  {
    DEBUG("LM ~LogManager");
  }

  void LogManager::log(const LogMessage& msg)
  {
    DEBUG("LM:log " << _listeners.size());
    for (int listenerIt = 0; listenerIt < _listeners.size();)
    {
      bool remove = true;
      if (boost::shared_ptr<LogListener> l = _listeners[listenerIt].lock())
      {
        DEBUG("LM::log listener lock");
        l->log(msg);
        remove = false;
      }

      if (remove)
      {
        std::swap(_listeners[_listeners.size() - 1], _listeners[listenerIt]);
        _listeners.pop_back();
      }
      else
      {
        ++listenerIt;
      }
    }
    DEBUG("LM::log exit " << _listeners.size());
  }


  LogListenerPtr LogManager::getListener()
  {
    DEBUG("LM getListener");

    LogListenerPtr ptr = boost::make_shared<LogListener>(boost::ref(*this));
    boost::weak_ptr<LogListener> l(ptr);
    _listeners.push_back(l);

    DEBUG("LM getListener ptr: " << ptr << " listener: " << ptr.get());
    return ptr;
  }

  void LogManager::recomputeVerbosities(qi::LogLevel from,
                                        qi::LogLevel to)
  {
    DEBUG("LM recomputeVerbosities");
    if (_maxLevel >= from && _maxLevel >= to)
    {
      return;
    }

    qi::LogLevel newMax = qi::LogLevel_Silent;

    for (int listenerIt = 0; listenerIt < _listeners.size();)
    {
      bool remove = true;
      if (boost::shared_ptr<LogListener> l = _listeners[listenerIt].lock())
      {
        newMax = std::max(newMax, l->verbosity.get());
        remove = false;
      }

      if (remove)
      {
        std::swap(_listeners[_listeners.size() - 1], _listeners[listenerIt]);
        _listeners.pop_back();
      }
      else
      {
        ++listenerIt;
      }
    }

    DEBUG("LM recomputed verbosity " << newMax);
    if (newMax != _maxLevel)
    {
      _maxLevel = newMax;
      for (unsigned i = 0; i < _providers.size(); ++i)
      {
        bool remove = true;
        if (LogProviderProxyPtr p = _providers[i].lock())
        {
          p->setVerbosity(_maxLevel);
          remove = false;
        }

        if (remove)
        {
          std::swap(_providers[_providers.size() - 1], _providers[i]);
          _providers.pop_back();
        }
        else
        {
          ++i;
        }
      }
    }
  }

  void LogManager::addProvider(LogProviderProxyPtr provider)
  {
    DEBUG("LM addProvider");
    _providers.push_back(boost::weak_ptr<LogProviderProxy>(provider));
    provider->setVerbosity(_maxLevel).async();
    provider->clearAndSet(_filters).async();
  }

  void LogManager::recomputeCategories()
  {
    DEBUG("LM recomputeCategories");
    // Soon you will know why this function is at the end of the source file...
    // Merge requests in one big map, keeping most verbose, ignoring globbing
    // Then, make a second pass that removes rules that overrides others and reduce verbosity
    if (_listeners.size() == 1)
    {
      // easy case
      _filters.clear();
      if (boost::shared_ptr<LogListener> l = _listeners.front().lock())
      {
        _filters.insert(_filters.begin(), l->_filters.begin(), l->_filters.end());
        for (unsigned i = 0; i < _providers.size(); ++i)
        {
          bool remove = true;
          if (LogProviderProxyPtr p = _providers[i].lock())
          {
            p->clearAndSet(_filters).async();
            remove = false;
          }

          if (remove)
          {
            std::swap(_providers[_providers.size() - 1], _providers[i]);
            _providers.pop_back();
          }
          else
          {
            ++i;
          }
        }
      }
      else
      {
        _listeners.clear();
      }
      return;
    }
    _filters.clear();
    typedef LogListener::FilterMap FilterMap;
    FilterMap map;

    for (int listenerIt = 0; listenerIt < _listeners.size();)
    {
      bool remove = true;
      if (boost::shared_ptr<LogListener> l = _listeners[listenerIt].lock())
      {
        remove = false;
        for (FilterMap::iterator it = l->_filters.begin();
             it != l->_filters.end();
             ++it)
        {
          // If we find a glob that has an other pattern than 'foo*', bailout
          size_t star = it->first.find('*');
          if (star != it->first.npos && star < it->first.length() - 1)
          {
            goto bailout;
          }

          FilterMap::iterator found = map.find(it->first);
          if (found == map.end())
          {
            map[it->first] = it->second;
          }
          else
          {
            found->second = std::max(found->second, it->second);
          }
        }
      }

      if (remove)
      {
        std::swap(_listeners[_listeners.size() - 1], _listeners[listenerIt]);
        _listeners.pop_back();
      }
      else
      {
        ++listenerIt;
      }
    }

    // Now remove overriding rules that reduce verbosity
    // This is a O(n2) algorithm that could be optimized:
    // - Check only entries with globbing
    // - Build a dependency graph.
    // But we expect only a small number of filter rules to be present.
    for (FilterMap::iterator it = map.begin(); it != map.end();)
    {
      bool remove = false;
      for (FilterMap::iterator it2 = map.begin(); it2 != it; ++it2)
      {
        if (it2->first.find('*') != it2->first.npos
            && qi::os::fnmatch(it2->first, it->first)
            && it2->second > it->second)
        {
          remove = true;
          break;
        }
      }

      if (remove)
      {
        FilterMap::iterator tmp = it;
        ++tmp;
        map.erase(it);
        it = tmp;
      }
      else
      {
        ++it;
      }
    }

    _filters.insert(_filters.end(), map.begin(), map.end());
bailout:
    for (unsigned i = 0; i < _providers.size(); ++i)
    {
      bool remove = true;
      if (LogProviderProxyPtr p = _providers[i].lock())
      {
        p->clearAndSet(_filters).async();
        remove = false;
      }

      if (remove)
      {
        std::swap(_providers[_providers.size() - 1], _providers[i]);
        _providers.pop_back();
      }
      else
      {
        ++i;
      }
    }
  }
} // !qi

#include <qicore/logmanager_bind.hpp>
