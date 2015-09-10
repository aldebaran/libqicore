/*
**  Copyright (C) 2012 Aldebaran Robotics
**  See COPYING for the license
*/

#include <qicore/file.hpp>

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <qi/anymodule.hpp>
#include <qi/eventloop.hpp>

#include "progressnotifier_p.hpp"

namespace qi
{
typedef ProgressNotifierBaseImpl<FileOperation> FileOperationBaseImpl;
typedef boost::function<Future<void>()> OperationLauncher;

class FileOperationImpl : public FileOperationBaseImpl
{
public:
  FileOperationImpl(OperationLauncher operationLauncher)
    : _opLauncher(std::move(operationLauncher))
  {
  }

  Future<void> start() override
  {
    OperationLauncher op = std::move(_opLauncher);
    assert(!_opLauncher);
    _opFuture = op();
    return _opFuture;
  }

private:
  OperationLauncher _opLauncher;
};


template<class TaskType, class... OpArgs>
FileOperationPtr makeFileOperation(OpArgs&&... opArgs)
{
  auto task = boost::make_shared<TaskType>(std::forward<OpArgs>(opArgs)...);
  auto fileOp = boost::make_shared<FileOperationImpl>( [task]{ return task->start(); } );
  task->setFileOperation(fileOp);
  return fileOp;
}

namespace
{
  class FileTransferTask : public boost::enable_shared_from_this<FileTransferTask>
  {
  public:
    FileTransferTask(FilePtr sourceFile, const Path& localPath, ProgressNotifierPtr remoteNotifier)
      : _sourceFile(std::move(sourceFile))
      , _fileSize(sourceFile->size())
      , _bytesWritten(0)
      , _remoteNotifier(std::move(remoteNotifier))
      , _localPath(localPath)
      , _promise(PromiseNoop<void>)
    {
    }

    void setFileOperation(FileOperationPtr fileOp)
    {
      assert(fileOp);
      _localNotifier = fileOp;
    }

    Future<void> start()
    {
      auto myself = shared_from_this();
      async<void>([myself]{ return myself->launch(); });
      return _promise.future();
    }

  private:
    boost::filesystem::ofstream _localFile;
    FilePtr _sourceFile;
    std::streamsize _fileSize;
    std::streamsize _bytesWritten;
    FileOperationPtr _localNotifier;
    ProgressNotifierPtr _remoteNotifier;
    const Path _localPath;
    Promise<void> _promise;

    void launch()
    {
      _localNotifier->_reset();
      _remoteNotifier->_reset();
      _localNotifier->_notifyRunning();
      _remoteNotifier->_notifyRunning();
      if (makeLocalFile())
      {
        fetchData();
      }
    }

    bool makeLocalFile()
    {
      _localFile.open(_localPath.bfsPath(), std::ios::out | std::ios::binary);
      if (!_localFile.is_open())
      {
        _localNotifier->_notifyFailed();
        _remoteNotifier->_notifyFailed();
        _promise.setError("Failed to create local file copy.");
        return false;
      }
      return true;
    }

    void write(Buffer buffer)
    {
      assert(_localFile.is_open());
      _localFile.write(static_cast<const char*>(buffer.data()), buffer.totalSize());
      _bytesWritten += buffer.totalSize();
      assert(_fileSize >= _bytesWritten);

      const double progress = static_cast<double>(_bytesWritten) / static_cast<double>(_fileSize);
      _localNotifier->_notifyProgressed(progress);
      _remoteNotifier->_notifyProgressed(progress);
    }

    void fetchData()
    {
      static const size_t ARBITRARY_BYTES_TO_READ_PER_CYCLE = 512 * 1024;
      auto myself = shared_from_this();
      _sourceFile.async<Buffer>("_read", _bytesWritten, ARBITRARY_BYTES_TO_READ_PER_CYCLE)
        .connect([this, myself](Future<Buffer> futureBuffer)
          {
            if (futureBuffer.hasError())
            {
              fail(futureBuffer.error());
              return;
            }
            else if (_promise.isCancelRequested())
            {
              cancel();
              return;
            }

            if (_bytesWritten < _fileSize)
            {
              write(futureBuffer.value());
              fetchData();
            }
            else
            {
              stop();
            }
          }
        );
    }

    void stop()
    {
      _localFile.close();
      _promise.setValue(0);
      _localNotifier->_notifyFinished();
      _remoteNotifier->_notifyFinished();
    }

    void fail(const std::string& errorMessage)
    {
      _promise.setError(errorMessage);
      clearLocalFile();
      _localNotifier->_notifyFailed();
      _remoteNotifier->_notifyFailed();
    }

    void cancel()
    {
      clearLocalFile();
      _promise.setCanceled();
      _localNotifier->_notifyCanceled();
      _remoteNotifier->_notifyCanceled();
    }

    void clearLocalFile()
    {
      _localFile.close();
      boost::filesystem::remove(_localPath);
    }
  };

}

FileOperationPtr prepareCopyToLocal(FilePtr sourceFile, const Path& localPath)
{
  return makeFileOperation<FileTransferTask>(sourceFile, localPath, sourceFile->operationProgress());
}

FutureSync<void> copyToLocal(FilePtr file, const Path& localPath)
{
  return prepareCopyToLocal(file, localPath)->start();
}
}
