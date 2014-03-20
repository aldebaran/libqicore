
#ifndef _LOGMANAGER_PROXY_HPP_
#define _LOGMANAGER_PROXY_HPP_

#include <vector>
#include <string>
#include <map>

#include <qi/types.hpp>
#include <qitype/signal.hpp>
#include <qitype/property.hpp>
#include <qitype/anyobject.hpp>
#include <qitype/proxysignal.hpp>
#include <qitype/proxyproperty.hpp>
#include <qicore/logmessage.hpp>


namespace qi
{


class LogManagerProxy;
typedef boost::shared_ptr<LogManagerProxy> LogManagerProxyPtr;
#ifndef FWD_LogListener
class LogListenerProxy; typedef boost::shared_ptr<LogListenerProxy> LogListenerProxyPtr;
#endif


class LogManagerProxy: public ::qi::Proxy
{
public:
  LogManagerProxy(qi::AnyObject obj)
  : qi::Proxy(obj)
  {

  }
   public:
  qi::FutureSync<void > log(LogMessage p0, ::qi::MetaCallType callType = ::qi::MetaCallType_Auto) {
    return _obj.call<void >(callType,"log", p0);
  }
  qi::FutureSync<LogListenerProxyPtr > getListener(::qi::MetaCallType callType = ::qi::MetaCallType_Auto) {
    return _obj.call<LogListenerProxyPtr >(callType,"getListener");
  }
  qi::FutureSync<int > addProvider(::qi::AutoAnyReference p0, ::qi::MetaCallType callType = ::qi::MetaCallType_Auto) {
    return _obj.call<int >(callType,"addProvider", p0);
  }
  qi::FutureSync<void > removeProvider(::qi::AutoAnyReference p0, ::qi::MetaCallType callType = ::qi::MetaCallType_Auto) {
    return _obj.call<void >(callType,"removeProvider", p0);
  }


};

QI_REGISTER_PROXY(LogManagerProxy);
} // !qi


#endif //_LOGMANAGER_PROXY_HPP_
