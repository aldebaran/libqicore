/*
** Author(s):
**  - Herve Cuche <hcuche@aldebaran-robotics.com>
**  - Matthieu Nottale <mnottale@aldebaran-robotics.com>
**
** Copyright (C) 2013 Aldebaran Robotics
*/

#include <boost/make_shared.hpp>

#include <logger/logger.hpp>
#include "src/loggermanager.hpp"

#include <qi/os.hpp>

#include <services/logger/logprovider_proxy.hpp>

static bool debug = getenv("LOGGER_DEBUG");
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

bool set_verbosity(LogListener* ll,
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
LogListener::LogListener(LoggerManager& l)
  : _logger(l)
  , verbosity(qi::Property<qi::LogLevel>::Getter(),
              boost::bind(&set_verbosity, this, _1, _2))
{
  verbosity.set(qi::LogLevel_Debug);
  onMessage.setCallType(qi::MetaCallType_Queued);
}

void LogListener::setVerbosity(qi::LogLevel level)
{
  DEBUG("LL verbosity " << level);
  verbosity.set(level);
}

void LogListener::clearFilters()
{
  _filters.clear();
  _logger.recomputeCategories();
}

void LogListener::setCategory(const std::string& cat,
                              qi::LogLevel level)
{
  _filters[cat] = level;
  _logger.recomputeCategories();
}

void LogListener::log(const Message& msg)
{
  DEBUG("LL:log");
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
    onMessage(msg);
}



LoggerManagerPtr make_LoggerPtr()
{
  return boost::make_shared<LoggerManager>();
}

// LoggerManager Class
LoggerManager::LoggerManager()
  : _maxLevel(qi::LogLevel_Silent)
{
  DEBUG("LoggerManager instanciating");
}

void LoggerManager::log(const Message& msg)
{
  DEBUG("LoggerManager::log " << _listeners.size());
  for (unsigned i = 0; i < _listeners.size(); ++i)
  {
    _listeners[i]->log(msg);
  }
}

LogListenerPtr LoggerManager::getListener()
{
  LogListenerPtr ptr = boost::make_shared<LogListener>(boost::ref(*this));
  _listeners.push_back(ptr);

  return ptr;
}

void LoggerManager::recomputeVerbosities(qi::LogLevel from,
                                         qi::LogLevel to)
{
  if (_maxLevel >= from && _maxLevel >= to)
  {
    return;
  }

  qi::LogLevel newMax = qi::LogLevel_Silent;
  for (unsigned i = 0; i < _listeners.size(); ++i)
  {
    newMax = std::max(newMax, _listeners[i]->verbosity.get());
  }

  DEBUG("recomputed verbosity " << newMax);
  if (newMax != _maxLevel)
  {
    _maxLevel = newMax;
    for (unsigned i = 0; i < _providers.size(); ++i)
    {
      _providers[i]->setVerbosity(_maxLevel);
    }
  }
}

void LoggerManager::addProvider(LogProviderProxyPtr provider)
{
  DEBUG("LoggerManager::addProvider");
  _providers.push_back(provider);
  provider->setVerbosity(_maxLevel).async();
  provider->clearAndSet(_filters).async();
}

void LoggerManager::recomputeCategories()
{
  // Soon you will know why this function is at the end of the source file...
  // Merge requests in one big map, keeping most verbose, ignoring globbing
  // Then, make a second pass that removes rules that overrides others and reduce verbosity
  if (_listeners.size() == 1)
  {
    // easy case
    _filters.clear();
    _filters.insert(_filters.begin(), _listeners.front()->_filters.begin(), _listeners.front()->_filters.end());
    for (unsigned i = 0; i < _providers.size(); ++i)
    {
      _providers[i]->clearAndSet(_filters).async();
    }
    return;
  }

  _filters.clear();
  typedef LogListener::FilterMap FilterMap;
  FilterMap map;
  for (unsigned i = 0; i < _listeners.size(); ++i)
  {
    for (FilterMap::iterator it = _listeners[i]->_filters.begin();
         it != _listeners[i]->_filters.end();
         ++i)
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
    _providers[i]->clearAndSet(_filters).async();
  }
}

#include "loggermanager_bind.hpp"
