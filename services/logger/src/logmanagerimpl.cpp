/*
** Author(s):
**  - Herve Cuche <hcuche@aldebaran-robotics.com>
**  - Matthieu Nottale <mnottale@aldebaran-robotics.com>
**
** Copyright (C) 2013 Aldebaran Robotics
*/

#include <qi/type/objecttypebuilder.hpp>

#include <qicore/logmanager.hpp>

#include <qi/anymodule.hpp>

#include "src/logmanagerimpl.hpp"
#include "src/loglistenerimpl.hpp"

QI_TYPE_INTERFACE(LogManager);

#define MAX_MSGS_BUFFERS 500

qiLogCategory("log.manager");

static bool debug = getenv("LOG_DEBUG");
#define DEBUG(a)                                \
  do {                                          \
    if (debug) std::cerr << a << std::endl;     \
  } while(0)

static qi::Atomic<unsigned int> msgId;

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
  // LogManagerImpl Class
  LogManagerImpl::LogManagerImpl()
    : _maxLevel(qi::log::logLevel())
  {
    DEBUG("LM instanciating");
    _historyMessages.rset_capacity(MAX_MSGS_BUFFERS);
  }

  LogManagerImpl::~LogManagerImpl()
  {
    DEBUG("LM ~LogManager");
  }

  void LogManagerImpl::pushBacklog(qi::LogListener* listener)
  {
    boost::mutex::scoped_lock dataLock(_dataMutex);
    std::vector<qi::LogMessage> historyLog;
    for (unsigned int i = 0; i < _historyMessages.size(); ++i)
      historyLog.push_back(_historyMessages[i]);

    listener->onLogMessagesWithBacklog(historyLog);
  }

  void LogManagerImpl::log(const std::vector<LogMessage>& msgs)
  {
    boost::mutex::scoped_lock dataLock(_dataMutex);
    DEBUG("LM:log listeners' numbers " << _listeners.size());
    DEBUG("LM:log providers' numbers " << _providers.size());
    DEBUG("LM:log MSG' numbers " << msgs.size());
    for (unsigned int msgsIt = 0; msgsIt < msgs.size(); ++msgsIt)
    {
      DEBUG("LM:log MSG' it " << msgsIt << " value: " << msgs[msgsIt].message);
      _historyMessages.push_back(msgs[msgsIt]);

      for (unsigned int listenerIt = 0; listenerIt < _listeners.size();)
      {
        DEBUG("LM:log listener it " << listenerIt);
        bool remove = true;
        if (boost::shared_ptr<LogListenerImpl> l = _listeners[listenerIt].lock())
        {
          DEBUG("LM:log listener log " << listenerIt);
          qi::LogMessage msg = msgs[msgsIt];
          msg.id = ++msgId;
          l->log(msg);
          remove = false;
        }

        if (remove)
        {
          DEBUG("LM:log listener " << listenerIt << " remove.");
          std::swap(_listeners[_listeners.size() - 1], _listeners[listenerIt]);
          _listeners.pop_back();
        }
        else
        {
          ++listenerIt;
        }
      }
    }
    DEBUG("LM:log done");
  }


  LogListenerPtr LogManagerImpl::getListener()
  {
    DEBUG("LM getListener");
    boost::shared_ptr<LogListenerImpl> ptr =
        boost::make_shared<LogListenerImpl>(boost::ref(*this),
                                            boost::bind(&LogManagerImpl::pushBacklog, this, _1));
    boost::weak_ptr<LogListenerImpl> l(ptr);

    boost::mutex::scoped_lock dataLock(_dataMutex);
    _listeners.push_back(l);

    DEBUG("LM getListener ptr: " << ptr);
    return ptr;
  }

  void LogManagerImpl::recomputeVerbosities(qi::LogLevel from,
                                            qi::LogLevel to)
  {
    DEBUG("LM recomputeVerbosities");
    // clean object sync
    gcProviders();
    if (_maxLevel >= from && _maxLevel >= to)
      return;

    qi::LogLevel newMax = qi::log::logLevel();

    {
      boost::mutex::scoped_lock dataLock(_dataMutex);
      for (unsigned int listenerIt = 0; listenerIt < _listeners.size();)
      {
        bool remove = true;
        if (boost::shared_ptr<LogListenerImpl> l = _listeners[listenerIt].lock())
        {
          newMax = std::max(newMax, l->logLevel.get());
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

      if (newMax != _maxLevel)
      {
        _maxLevel = newMax;
        std::map<int, LogProviderPtr >::iterator providersIt;
        for (providersIt = _providers.begin(); providersIt != _providers.end(); ++providersIt)
        {
          providersIt->second.async<void>("setLevel", _maxLevel).connect(&LogManagerImpl::providerCallback, this, _1, providersIt->first);
        }
      }
    }
  }

  int LogManagerImpl::addProvider(LogProviderPtr provider)
  {
    DEBUG("LM addProvider this: " << &provider);
    boost::mutex::scoped_lock dataLock(_dataMutex);
    int id = ++_providerId;
    _providers[id] = provider;

    provider.async<void>("setLevel", _maxLevel).connect(&LogManagerImpl::providerCallback, this, _1, id);
    provider.async<void>("setFilters", _filters).connect(&LogManagerImpl::providerCallback, this, _1, id);

    return id;
  }

  void LogManagerImpl::providerCallback(qi::Future<void> fut, int idProvider)
  {
    DEBUG("LM providerCallback id " << idProvider);
    if (fut.hasError())
    {
      boost::mutex::scoped_lock invalidProviderIdsLock(_invalidProviderIdsMutex);
      _invalidProviderIds.insert(idProvider);
    }

    return;
  }


  void LogManagerImpl::gcProviders()
  {
    DEBUG("LM gcProviders");
    boost::mutex::scoped_lock invalidProviderIdsLock(_invalidProviderIdsMutex);
    boost::mutex::scoped_lock dataLock(_dataMutex);
    std::set<int>::const_iterator invalidProviderIdsIt;
    for (invalidProviderIdsIt = _invalidProviderIds.begin();
         invalidProviderIdsIt != _invalidProviderIds.end();
         ++invalidProviderIdsIt)
    {
      _providers.erase(*invalidProviderIdsIt);
    }

    _invalidProviderIds.clear();
    return;
  }

  void LogManagerImpl::removeProvider(int idProvider)
  {
    DEBUG("LM removeProvider id " << idProvider);
    boost::mutex::scoped_lock dataLock(_dataMutex);
    _providers.erase(idProvider);
    _providers[idProvider].reset();
  }

  void LogManagerImpl::recomputeCategories()
  {
    DEBUG("LM recomputeCategories");

    // clean object sync
    gcProviders();
    // Soon you will know why this function is at the end of the source file...
    // Merge requests in one big map, keeping most verbose, ignoring globbing
    // Then, make a second pass that removes rules that overrides others and reduce verbosity
    typedef LogListenerImpl::FilterMap FilterMap;
    FilterMap map;
    {
      boost::mutex::scoped_lock dataLock(_dataMutex);
      if (_listeners.size() == 1)
      {
        // easy case
        _filters.clear();
        if (boost::shared_ptr<LogListenerImpl> l = _listeners.front().lock())
        {
          FilterMap listenerFilters = l->filters();
          _filters.insert(_filters.begin(), listenerFilters.begin(), listenerFilters.end());
          std::map<int, LogProviderPtr >::iterator providersIt;
          for (providersIt = _providers.begin();
               providersIt != _providers.end();
               ++providersIt)
          {
            providersIt->second.async<void>("setFilters", _filters).connect(&LogManagerImpl::providerCallback, this, _1, providersIt->first);
          }
        }
        else
        {
          _listeners.clear();
        }
        return;
      }
      _filters.clear();

      for (unsigned int listenerIt = 0; listenerIt < _listeners.size();)
      {
        bool remove = true;
        if (boost::shared_ptr<LogListenerImpl> l = _listeners[listenerIt].lock())
        {
          remove = false;
          FilterMap listenerFilters = l->filters();
          for (FilterMap::iterator it = listenerFilters.begin();
               it != listenerFilters.end();
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

    {
      boost::mutex::scoped_lock dataLock(_dataMutex);
      _filters.insert(_filters.end(), map.begin(), map.end());
    }
bailout:
    boost::mutex::scoped_lock dataLock(_dataMutex);
    std::map<int, LogProviderPtr >::iterator providersIt;
    for (providersIt = _providers.begin(); providersIt != _providers.end(); ++providersIt)
    {
      providersIt->second.async<void>("setFilters", _filters).connect(&LogManagerImpl::providerCallback, this, _1, providersIt->first);
    }
  }

  QI_REGISTER_MT_OBJECT(LogManager, log, getListener, addProvider, removeProvider);
  QI_REGISTER_IMPLEMENTATION(LogManager, LogManagerImpl);

  void registerLogManager(ModuleBuilder* mb) {
    mb->advertiseFactory<LogManagerImpl>("LogManager");
    registerLogListener(mb);
  }
  QI_REGISTER_MODULE("logmanager", registerLogManager);

} // !qi
