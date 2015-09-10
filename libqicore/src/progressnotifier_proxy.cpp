#include <qicore/file.hpp>
#include <qi/anymodule.hpp>

namespace qi
{
class ProgressNotifierProxy : public ProgressNotifier, public qi::Proxy
{
public:
  ProgressNotifierProxy(qi::AnyObject obj)
    : qi::Proxy(std::move(obj))
  {
  }

  void _reset() override
  {
    _obj.call<void>("_reset");
  }

  void _notifyRunning() override
  {
    _obj.call<void>("_notifyRunning");
  }

  void _notifyFinished() override
  {
    _obj.call<void>("_notifyFinished");
  }

  void _notifyCanceled() override
  {
    _obj.call<void>("_notifyCanceled");
  }

  void _notifyFailed() override
  {
    _obj.call<void>("_notifyFailed");
  }

  void _notifyProgressed(double newProgress) override
  {
    _obj.call<void>("_notifyProgressed", newProgress);
  }

  bool isRunning() const override
  {
    return _obj.call<bool>("isRunning");
  }

  Future<void> waitForFinished() override
  {
    return _obj.async<void>("waitForFinished");
  }
};
QI_REGISTER_PROXY_INTERFACE(ProgressNotifierProxy, ProgressNotifier);
}
