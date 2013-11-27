
#ifndef _LOGLISTENER_PROXY_HPP_
#define _LOGLISTENER_PROXY_HPP_

#include <vector>
#include <string>
#include <map>

#include <qi/types.hpp>
#include <qitype/signal.hpp>
#include <qitype/property.hpp>
#include <qitype/anyobject.hpp>
#include <qitype/proxysignal.hpp>
#include <qitype/proxyproperty.hpp>



namespace qi
{


class LogListenerProxy;
typedef boost::shared_ptr<LogListenerProxy> LogListenerProxyPtr;


class LogListenerProxy: public ::qi::Proxy
{
public:
  LogListenerProxy(qi::AnyObject obj)
  : qi::Proxy(obj)
  {
    qi::makeProxySignal(onLogMessage, obj, "onLogMessage");
    qi::makeProxyProperty(verbosity, obj, "verbosity");

  }
   public:
  qi::FutureSync<void > setVerbosity(qi::LogLevel p0, ::qi::MetaCallType callType = ::qi::MetaCallType_Auto) {
    return _obj.call<void >(callType,"setVerbosity", p0);
  }
  qi::FutureSync<void > setCategory(std::string p0, qi::LogLevel p1, ::qi::MetaCallType callType = ::qi::MetaCallType_Auto) {
    return _obj.call<void >(callType,"setCategory", p0,p1);
  }
  qi::FutureSync<void > clearFilters(::qi::MetaCallType callType = ::qi::MetaCallType_Auto) {
    return _obj.call<void >(callType,"clearFilters");
  }
  qi::ProxySignal<void(LogMessage)> onLogMessage;
  qi::ProxyProperty<qi::LogLevel> verbosity;



};

QI_REGISTER_PROXY(LogListenerProxy);
} // !qi


QI_TYPE_PROXY(qi::LogListenerProxy);

#endif //_LOGLISTENER_PROXY_HPP_
