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

  void reset() override
  {
    _obj.call<void>("reset");
  }

  void notifyRunning() override
  {
    _obj.call<void>("notifyRunning");
  }

  void notifyFinished() override
  {
    _obj.call<void>("notifyFinished");
  }

  void notifyCanceled() override
  {
    _obj.call<void>("notifyCanceled");
  }

  void notifyFailed() override
  {
    _obj.call<void>("notifyFailed");
  }

  void notifyProgressed(double newProgress) override
  {
    _obj.call<void>("notifyProgressed", newProgress);
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
