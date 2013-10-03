/*
** Author(s):
**  - Herve Cuche <hcuche@aldebaran-robotics.com>
**  - Matthieu Nottale <mnottale@aldebaran-robotics.com>
**
** Copyright (C) 2013 Aldebaran Robotics
*/

#ifndef LOGMANAGER_HPP_
# define LOGMANAGER_HPP_

# include <boost/shared_ptr.hpp>

# include <qitype/signal.hpp>
# include <qitype/property.hpp>

# include <qicore/logmessage.hpp>
# include <qicore/logprovider_proxy.hpp>
# include <qicore/loglistener_proxy.hpp>

# include <map>
# include <string>
# include <utility>
# include <vector>

namespace qi
{
  class LogManager;
  typedef boost::shared_ptr<LogManager> LogManagerPtr;

  class LogListener;
  typedef boost::shared_ptr<LogListener> LogListenerPtr;

  /// implemented service side, client have proxies. @threadSafe.
  class LogListener
  {
    public:
      explicit LogListener(LogManager& l);
      ~LogListener();

      void setVerbosity(qi::LogLevel level);
      void setCategory(const std::string& cat,
                       qi::LogLevel level);

      /// Remove all filters set by setCategory
      void clearFilters();

    private:
      typedef std::map<std::string, qi::LogLevel> FilterMap;
      FilterMap      _filters;
      LogManager&    _logger;

    public:
      qi::Signal<LogMessage> onLogMessage;
      qi::Property<qi::LogLevel> verbosity;

    private:
      friend class LogManager;
      friend bool set_verbosity(LogListener* ll,
                                qi::LogLevel& level,
                                const qi::LogLevel& newvalue);

      void log(const LogMessage& m);
  };

  /// @threadSafe
  class LogManager
  {
    public:
      LogManager();
      ~LogManager();

      void log(const LogMessage& msg);
      LogListenerPtr getListener();
      void addProvider(LogProviderProxyPtr provider);

    private:
      void recomputeCategories();
      void recomputeVerbosities(qi::LogLevel from,
                                qi::LogLevel to);

    private:
      qi::LogLevel _maxLevel;
      std::vector<std::pair<std::string, qi::LogLevel> > _filters;

      std::vector<boost::weak_ptr<LogListener> >      _listeners;
      std::vector<boost::weak_ptr<LogProviderProxy> > _providers;

    private:
      friend class LogListener;
      friend bool set_verbosity(LogListener* ll,
                                qi::LogLevel& level,
                                const qi::LogLevel& newvalue);
  };
} // !qi

QI_TYPE_NOT_CLONABLE(qi::LogListener);

#endif // !LOGMANAGER_HPP_
