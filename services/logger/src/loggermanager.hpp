/*
** Author(s):
**  - Herve Cuche <hcuche@aldebaran-robotics.com>
**  - Matthieu Nottale <mnottale@aldebaran-robotics.com>
**
** Copyright (C) 2013 Aldebaran Robotics
*/

#ifndef LOGGERMANAGER_HPP_
# define LOGGERMANAGER_HPP_

# include <boost/shared_ptr.hpp>

# include <qitype/signal.hpp>
# include <qitype/property.hpp>

# include <logger/logger.hpp>
# include <services/logger/logprovider_proxy.hpp>
# include <services/logger/loglistener_proxy.hpp>

# include <map>
# include <string>
# include <utility>
# include <vector>

class LoggerManager;
typedef boost::shared_ptr<LoggerManager> LoggerManagerPtr;

class LogListener;
typedef boost::shared_ptr<LogListener> LogListenerPtr;

/// implemented service side, client have proxies. @threadSafe.
class LogListener
{
public:
  explicit LogListener(LoggerManager& l);

  void setVerbosity(qi::LogLevel level);
  void setCategory(const std::string& cat,
                   qi::LogLevel level);

  /// Remove all filters set by setCategory
  void clearFilters();

private:
  typedef std::map<std::string, qi::LogLevel> FilterMap;
  FilterMap      _filters;
  LoggerManager& _logger;

public:
  qi::Signal<Message> onMessage;
  qi::Property<qi::LogLevel> verbosity;

private:
  friend class LoggerManager;
  friend bool set_verbosity(LogListener* ll,
                            qi::LogLevel& level,
                            const qi::LogLevel& newvalue);

  void log(const Message& m);
};

QI_TYPE_NOT_CLONABLE(LogListener);


/// @threadSafe
class LoggerManager
{
public:
  LoggerManager();

  void log(const Message& msg);
  LogListenerPtr getListener();
  void addProvider(LogProviderProxyPtr provider);

private:
  void recomputeCategories();
  void recomputeVerbosities(qi::LogLevel from,
                            qi::LogLevel to);

private:
  qi::LogLevel _maxLevel;
  std::vector<std::pair<std::string, qi::LogLevel> > _filters;

  /// FIXME keep a week-ptr
  std::vector<LogListenerPtr>      _listeners;
  std::vector<LogProviderProxyPtr> _providers;

private:
  friend class LogListener;
  friend bool set_verbosity(LogListener* ll,
                            qi::LogLevel& level,
                            const qi::LogLevel& newvalue);
};

#endif // !LOGGERMANAGER_HPP_
