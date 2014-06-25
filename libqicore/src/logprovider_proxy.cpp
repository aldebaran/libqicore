#include <vector>
#include <string>
#include <utility>

#include <qi/types.hpp>
#include <qi/anyobject.hpp>

#include <qicore/logprovider.hpp>

namespace qi
{
  class LogProviderProxy : public qi::Proxy, public LogProvider
  {
  public:
    LogProviderProxy(qi::AnyObject obj)
      : qi::Proxy(obj)
    {
    }

    void setLevel(qi::LogLevel p0)
    {
      _obj.call<void>("setLevel", p0);
    }

    void addFilter(const std::string&  p0, qi::LogLevel p1)
    {
      _obj.call<void>("addFilter", p0, p1);
    }

    void setFilters(const std::vector<std::pair<std::string, qi::LogLevel> >& p0)
    {
      _obj.call<void>("setFilters", p0);
    }
  };

  QI_REGISTER_PROXY_INTERFACE(LogProviderProxy, LogProvider);
} // !qi
