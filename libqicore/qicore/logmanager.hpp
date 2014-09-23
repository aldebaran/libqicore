/*
** Author(s):
**  - Herve Cuche <hcuche@aldebaran-robotics.com>
**  - Matthieu Nottale <mnottale@aldebaran-robotics.com>
**
** Copyright (C) 2013 Aldebaran Robotics
*/

#ifndef LOGMANAGER_HPP_
# define LOGMANAGER_HPP_

# include <qicore/api.hpp>
# include <qicore/logmessage.hpp>

# include <qi/anyobject.hpp>

namespace qi
{
  class LogListener;
  typedef qi::Object<LogListener> LogListenerPtr;
  class LogProvider;
  typedef qi::Object<LogProvider> LogProviderPtr;
  class QICORE_API LogManager
  {
  public:
    virtual ~LogManager() {};

    virtual void log(const std::vector<LogMessage>& msgs) = 0;
    virtual LogListenerPtr getListener() = 0;
    virtual int addProvider(LogProviderPtr provider) = 0;
    virtual void removeProvider(int idProvider) = 0;
  };

  typedef qi::Object<LogManager> LogManagerPtr;
} // !qi

namespace qi {
namespace detail {
template <>
struct QICORE_API ForceProxyInclusion<qi::LogManager>
{
  bool dummyCall();
};
}}

#endif // !LOGMANAGER_HPP_
