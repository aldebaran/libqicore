#include <qicore/file.hpp>
#include <qi/anymodule.hpp>

namespace qi
{
class ProgressNotifierProxy : public ProgressNotifier, public qi::Proxy
{
public:
  ProgressNotifierProxy(qi::AnyObject obj)
    : qi::Proxy(obj)
  {
  }

  void _reset()
  {
    _obj.call<void>("_reset");
  }

  void _notifyRunning()
  {
    _obj.call<void>("_notifyRunning");
  }

  void _notifyFinished()
  {
    _obj.call<void>("_notifyFinished");
  }

  void _notifyCancelled()
  {
    _obj.call<void>("_notifyCancelled");
  }

  void _notifyFailed()
  {
    _obj.call<void>("_notifyFailed");
  }

  void _notifyProgressed(double newProgress)
  {
    _obj.call<void>("_notifyProgressed", newProgress);
  }

  bool isRunning() const
  {
    return _obj.call<bool>("isRunning");
  }
};
QI_REGISTER_PROXY_INTERFACE(ProgressNotifierProxy, ProgressNotifier);
}
