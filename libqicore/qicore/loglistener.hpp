/*
** Author(s):
**  - Herve Cuche <hcuche@aldebaran-robotics.com>
**  - Matthieu Nottale <mnottale@aldebaran-robotics.com>
**
** Copyright (C) 2013 Aldebaran Robotics
*/

#ifndef LOGLISTENER_HPP_
# define LOGLISTENER_HPP_

# include <string>

# include <qi/log.hpp>

# include <qicore/api.hpp>
# include <qicore/logmanager.hpp>
# include <qicore/logmessage.hpp>

# include <qitype/signal.hpp>
# include <qitype/property.hpp>
# include <qitype/anyobject.hpp>

namespace qi
{
  class QICORE_API LogListener
  {
  public:
    LogListener() {};
    virtual ~LogListener() {};

    virtual void setLevel(qi::LogLevel level)  = 0;
    virtual void addFilter(const std::string& filter,
                           qi::LogLevel level) = 0;

    /// Remove all filters set by addFilter
    virtual void clearFilters() = 0;

  public:
    qi::Signal<qi::LogMessage> onLogMessage;
    qi::Property<qi::LogLevel> logLevel;

  protected:
    LogListener(qi::Property<qi::LogLevel>::Getter get,
                qi::Property<qi::LogLevel>::Setter set)
      : logLevel(get, set)
    {
    }
  };

  typedef qi::Object<LogListener> LogListenerPtr;
} // !qi


#endif // !LOGLISTENER_HPP_
