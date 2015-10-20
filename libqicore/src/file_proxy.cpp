#include <qicore/file.hpp>
#include <qi/anymodule.hpp>

namespace qi
{
class FileProxy : public File, public qi::Proxy
{
public:
  FileProxy(qi::AnyObject obj)
    : qi::Proxy(std::move(obj))
  {
  }

  ~FileProxy() = default;

  Buffer read(std::streamsize countBytesToRead) override
  {
    return _obj.call<Buffer>("read", countBytesToRead);
  }

  Buffer read(std::streamoff beginOffset, std::streamsize countBytesToRead) override
  {
    return _obj.call<Buffer>("read", beginOffset, countBytesToRead);
  }

  bool seek(std::streamoff offsetFromBegin) override
  {
    return _obj.call<bool>("seek", offsetFromBegin);
  }

  void close() override
  {
    return _obj.call<void>("close");
  }

  std::streamsize size() const override
  {
    return _obj.call<std::streamsize>("size");
  }

  bool isOpen() const override
  {
    return _obj.call<bool>("isOpen");
  }

  bool isRemote() const override
  {
    return true;
  }

  ProgressNotifierPtr operationProgress() const override
  {
    return _obj.call<ProgressNotifierPtr>("operationProgress");
  }

};

void _qiregisterFileProxy()
{
  ::qi::registerProxyInterface<FileProxy, File>();
}

}
