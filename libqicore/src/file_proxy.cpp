#include <qicore/file.hpp>
#include <qi/anymodule.hpp>

namespace qi
{
class FileProxy : public File, public qi::Proxy
{
public:
  FileProxy(qi::AnyObject obj)
    : qi::Proxy(obj)
  {
  }

  ~FileProxy()
  {
  }

  Buffer _read(std::streamsize countBytesToRead)
  {
    return _obj.call<Buffer>("_read", countBytesToRead);
  }

  Buffer _read(std::streamoff beginOffset, std::streamsize countBytesToRead)
  {
    return _obj.call<Buffer>("_read", beginOffset, countBytesToRead);
  }

  bool _seek(std::streamoff offsetFromBegin)
  {
    return _obj.call<bool>("_seek", offsetFromBegin);
  }

  void _close()
  {
    return _obj.call<void>("_close");
  }

  std::streamsize size() const
  {
    return _obj.call<std::streamsize>("size");
  }

  bool isOpen() const
  {
    return _obj.call<bool>("isOpen");
  }

  bool isRemote() const
  {
    return true;
  }

  ProgressNotifierPtr operationProgress() const
  {
    return _obj.call<ProgressNotifierPtr>("operationProgress");
  }
};
QI_REGISTER_PROXY_INTERFACE(FileProxy, File);
}
