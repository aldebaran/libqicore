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
    : _opLauncher(operationLauncher)
  {
  }

  Future<void> start()
  {
    OperationLauncher op = _opLauncher;
    _opLauncher.clear();
    _opFuture = op();
    return _opFuture;
  }

private:
  OperationLauncher _opLauncher;
};

namespace
{
  class FileTransferTask;
  typedef boost::shared_ptr<FileTransferTask> FileTransferTaskPtr;

  class FileTransferTask : public boost::enable_shared_from_this<FileTransferTask>
  {
  public:
    FileTransferTask(FilePtr sourceFile, const Path& localPath, ProgressNotifierPtr remoteNotifier)
      : _sourceFile(sourceFile)
      , _fileSize(sourceFile->size())
      , _bytesWritten(0)
      , _remoteNotifier(remoteNotifier)
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
      async<void>(&FileTransferTask::launch, shared_from_this());
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
        _localNotifier->_notifyCancelled();
        _remoteNotifier->_notifyCancelled();
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
      _sourceFile.async<Buffer>("_read", _bytesWritten, ARBITRARY_BYTES_TO_READ_PER_CYCLE)
          .connect(&FileTransferTask::decideNextStep, shared_from_this(), _1);
    }

    void decideNextStep(Future<Buffer> futureBuffer)
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
      _localFile.close();
      boost::filesystem::remove(_localPath.str());
      _localNotifier->_notifyFailed();
      _remoteNotifier->_notifyFailed();
    }

    void cancel()
    {
      _localFile.close();
      boost::filesystem::remove(_localPath.str());
      _promise.setCanceled();
      _localNotifier->_notifyCancelled();
      _remoteNotifier->_notifyCancelled();
    }
  };
}

FileOperationPtr prepareCopyToLocal(FilePtr sourceFile, const Path& localPath)
{
  FileTransferTaskPtr transferTask =
      boost::make_shared<FileTransferTask>(sourceFile, localPath, sourceFile->operationProgress());
  OperationLauncher opLauncher = boost::bind(&FileTransferTask::start, transferTask);
  FileOperationPtr fileOp = boost::make_shared<FileOperationImpl>(opLauncher);
  transferTask->setFileOperation(fileOp);
  return fileOp;
}

FutureSync<void> copyToLocal(FilePtr file, const Path& localPath)
{
  return prepareCopyToLocal(file, localPath)->start();
}
}
