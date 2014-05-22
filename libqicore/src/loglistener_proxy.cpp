#include <string>

#include <qi/types.hpp>

#include <qitype/anyobject.hpp>
#include <qitype/signal.hpp>
#include <qitype/property.hpp>
#include <qitype/proxysignal.hpp>
#include <qitype/proxyproperty.hpp>

#include <qicore/loglistener.hpp>
#include <qicore/logmessage.hpp>

namespace qi
{
  class LogListenerProxy : public qi::Proxy, public LogListener
  {
  public:
    LogListenerProxy(qi::AnyObject obj)
      : qi::Proxy(obj)
      , qi::LogListener()
    {
      qi::makeProxySignal(onLogMessage, obj, "onLogMessage");
      qi::makeProxyProperty(logLevel, obj, "logLevel");
    }

    void setLevel(qi::LogLevel p0)
    {
      _obj.call<void>("setLevel", p0);
    }

    void addFilter(const std::string& p0, qi::LogLevel p1)
    {
      _obj.call<void>("addFilter", p0, p1);
    }

    void clearFilters()
    {
      _obj.call<void>("clearFilters");
    }
  };

  QI_REGISTER_PROXY_INTERFACE(LogListenerProxy, LogListener);
} // !qi
