/*
** Author(s):
**  - Herve Cuche <hcuche@aldebaran-robotics.com>
**  - Matthieu Nottale <mnottale@aldebaran-robotics.com>
**
** Copyright (C) 2013 Aldebaran Robotics
*/

#ifndef LOGLISTENERIMPL_HPP_
# define LOGLISTENERIMPL_HPP_

# include <map>

# include <boost/thread/mutex.hpp>

# include <qicore/logmessage.hpp>
# include <qicore/logmanager.hpp>
# include <qicore/loglistener.hpp>

namespace qi
{
  class LogManagerImpl;
  /// implemented service side, client have proxies. @threadSafe.
  class LogListenerImpl : public LogListener
  {
  public:
    LogListenerImpl(LogManagerImpl& l);
    LogListenerImpl(LogManagerImpl& l,
                    boost::function<void (LogListener*)> func);

    virtual ~LogListenerImpl();

    virtual void setLevel(qi::LogLevel level);
    virtual void addFilter(const std::string& filter,
                           qi::LogLevel level);

    // Remove all filters set by addFilter
    virtual void clearFilters();

    typedef std::map<std::string, qi::LogLevel> FilterMap;
    FilterMap    _filters;
    boost::mutex _filtersMutex;

    LogManagerImpl& _logger;

    void log(const LogMessage& m);
    std::map<std::string, qi::LogLevel> filters();
  };

  class ModuleBuilder;
  void registerLogListener(qi::ModuleBuilder* mb);

} // !qi


#endif // !LOGLISTENERIMPL_HPP_
