/*
** Author(s):
**  - Herve Cuche <hcuche@aldebaran-robotics.com>
**  - Matthieu Nottale <mnottale@aldebaran-robotics.com>
**
** Copyright (C) 2013 Aldebaran Robotics
*/

#ifndef LOGMANAGERIMPL_HPP_
#define LOGMANAGERIMPL_HPP_

#include <map>
#include <utility>
#include <set>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/circular_buffer.hpp>

#include <qicore/logmanager.hpp>
#include <qicore/logmessage.hpp>
#include <qicore/loglistener.hpp>
#include <qicore/logprovider.hpp>

#include <qi/atomic.hpp>
#include <qi/future.hpp>
#include <qi/log.hpp>

#include <qi/detail/warn_push_ignore_deprecated.hpp>

namespace qi
{
class LogListenerImpl;
class LogManagerImpl : public LogManager
{
public:
  LogManagerImpl();
  ~LogManagerImpl() override;

  void log(const std::vector<LogMessage>& msgs) override;
  LogListenerPtr createListener() override;
  LogListenerPtr getListener() override;
  int addProvider(LogProviderPtr provider) override;
  void removeProvider(int idProvider) override;

  void providerCallback(qi::Future<void> fut, int idProvider);
  void gcProviders();

  void recomputeCategories();
  void recomputeVerbosities(qi::LogLevel from, qi::LogLevel to);

  void pushBacklog(LogListener* listener);

  qi::LogLevel _maxLevel;
  std::vector<std::pair<std::string, qi::LogLevel> > _filters;
  std::vector<boost::weak_ptr<LogListenerImpl> > _listeners;

  qi::Atomic<int> _providerId;
  std::set<int> _invalidProviderIds;
  boost::mutex _invalidProviderIdsMutex;
  std::map<int, LogProviderPtr> _providers;

  boost::mutex _dataMutex;
  boost::circular_buffer<qi::LogMessage> _historyMessages;
};
} // !qi

#include <qi/detail/warn_pop_ignore_deprecated.hpp>
#endif // !LOGMANAGERIMPL_HPP_
