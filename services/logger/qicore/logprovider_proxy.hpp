
#ifndef _LOGPROVIDER_PROXY_HPP_
#define _LOGPROVIDER_PROXY_HPP_

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


class LogProviderProxy;
typedef boost::shared_ptr<LogProviderProxy> LogProviderProxyPtr;


class LogProviderProxy: public ::qi::Proxy
{
public:
  LogProviderProxy(qi::AnyObject obj)
  : qi::Proxy(obj)
  {

  }
   public:
  qi::FutureSync<void > setVerbosity(qi::LogLevel p0, ::qi::MetaCallType callType = ::qi::MetaCallType_Auto) {
    return _obj.call<void >(callType,"setVerbosity", p0);
  }
  qi::FutureSync<void > setCategory(std::string p0, qi::LogLevel p1, ::qi::MetaCallType callType = ::qi::MetaCallType_Auto) {
    return _obj.call<void >(callType,"setCategory", p0,p1);
  }
  qi::FutureSync<void > clearAndSet(std::vector<std::pair<std::string,qi::LogLevel > > p0, ::qi::MetaCallType callType = ::qi::MetaCallType_Auto) {
    return _obj.call<void >(callType,"clearAndSet", p0);
  }



};

QI_REGISTER_PROXY(LogProviderProxy);
} // !qi


QI_TYPE_PROXY(qi::LogProviderProxy);

#endif //_LOGPROVIDER_PROXY_HPP_
