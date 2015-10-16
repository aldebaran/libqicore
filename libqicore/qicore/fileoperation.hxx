#pragma once
#ifndef _QICORE_FILEOPERATION_HPP_
#define _QICORE_FILEOPERATION_HPP_

#include <atomic>
#include <memory>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <qi/anymodule.hpp>
#include <qi/eventloop.hpp>

namespace qi
{
  /** Represent a file operation ready to be executed and expose information about it's progress state.
  *   Exposes the same signals than ProgressNotifier, associated to the operation.
  *   @includename{qicore/file.hpp}
  **/
  class FileOperation
  {
  public:
    ~FileOperation()
    {
      auto task = std::move(_task);
      if (task)
      {
        task->promise.future().cancel();
      }
    }

    // Move only
    FileOperation(const FileOperation&) = delete;
    FileOperation& operator=(const FileOperation&) = delete;
    FileOperation(FileOperation&& other) : _task(std::move(other._task)) {}  // TODO VS2015 C++11: = default;
    FileOperation& operator=(FileOperation&& other) // TODO VS2015 C++11: = default;
    {
      _task = std::move(other._task);
       return *this;
    }

    qi::Future<void> start()
    {
      acquireTask();
      return _task->run();
    }

    qi::Future<void> startStandAlone()
    {
      acquireTask();
      boost::shared_ptr<Task> sharedTask{ std::move(_task) };
      auto future = sharedTask->run();
      future.connect([sharedTask](Future<void>&){}); // keep the task alive until it ends
      return future;
    }

    auto operator()() -> decltype(start()) { return start(); }

    ProgressNotifierPtr notifier() const { return _task ? _task->localNotifier : ProgressNotifierPtr{}; }

    bool isValid() const { return _task ? true : false; }
    explicit operator bool() const { return isValid(); }

  protected:
    struct Task
      : public boost::enable_shared_from_this<Task>
    {
      Task(FilePtr file)
        : sourceFile{ std::move(file) }
        , fileSize{ sourceFile->size() }
        , promise{ PromiseNoop<void> }
        , remoteNotifier{ sourceFile->operationProgress() }
      {
      }

      virtual ~Task() = default;

      qi::Future<void> run()
      {
        localNotifier->_reset();
        remoteNotifier->_reset();
        localNotifier->_notifyRunning();
        remoteNotifier->_notifyRunning();
        start();
        return promise.future();
      }

      void finish()
      {
        promise.setValue(0);
        localNotifier->_notifyFinished();
        remoteNotifier->_notifyFinished();
      }

      void fail(const std::string& errorMessage)
      {
        promise.setError(errorMessage);
        localNotifier->_notifyFailed();
        remoteNotifier->_notifyFailed();
      }

      void cancel()
      {
        promise.setCanceled();
        localNotifier->_notifyCanceled();
        remoteNotifier->_notifyCanceled();
      }


      virtual void start() = 0;

      const FilePtr sourceFile;
      const std::streamsize fileSize;
      Promise<void> promise;
      const ProgressNotifierPtr localNotifier = createProgressNotifier(promise.future());
      const ProgressNotifierPtr remoteNotifier;
    };

    using TaskPtr = boost::shared_ptr<Task>;

    explicit FileOperation(TaskPtr task)
      : _task{ std::move(task) }
    {
      if (!_task)
        throw std::runtime_error("FileOperation requires a non-null task on constrution.");
    }

  private:
    TaskPtr _task;
    qi::Atomic<bool> isLaunched{ false };

    void acquireTask()
    {
      if (!_task)
      {
        throw std::runtime_error{ "Tried to start an invalid FileOperation" };
      }

      if (isLaunched.swap(true))
      {
        // TODO: Consider returning a future error instead.
        throw std::runtime_error{ "Called FileOperation::start() more than once!" };
      }
    }
  };

  class FileCopyToLocal
    : public FileOperation
  {
  public:
    FileCopyToLocal(qi::FilePtr file, qi::Path localPath)
      : FileOperation(boost::make_shared<Task>(std::move(file), std::move(localPath)))
    {
    }

  private:
    class Task
      : public FileOperation::Task
    {
    public:
      Task(FilePtr sourceFile, qi::Path localFilePath)
        : FileOperation::Task(std::move(sourceFile))
        , localPath(std::move(localFilePath))
      {
      }

      void start() override
      {
        if (makeLocalFile())
        {
          fetchData();
        }
      }

      void stop()
      {
        localFile.close();
        finish();
      }

      bool makeLocalFile()
      {
        localFile.open(localPath.bfsPath(), std::ios::out | std::ios::binary);
        if (!localFile.is_open())
        {
          fail("Failed to create local file copy.");
          return false;
        }
        return true;
      }

      void write(Buffer buffer)
      {
        assert(localFile.is_open());
        localFile.write(static_cast<const char*>(buffer.data()), buffer.totalSize());
        bytesWritten += buffer.totalSize();
        assert(fileSize >= bytesWritten);

        const double progress = static_cast<double>(bytesWritten) / static_cast<double>(fileSize);
        localNotifier->_notifyProgressed(progress);
        remoteNotifier->_notifyProgressed(progress);
      }

      void fetchData()
      {
        static const size_t ARBITRARY_BYTES_TO_READ_PER_CYCLE = 512 * 1024;
        auto myself = shared_from_this();
        sourceFile.async<Buffer>("_read", bytesWritten, ARBITRARY_BYTES_TO_READ_PER_CYCLE)
          .connect([this, myself](Future<Buffer> futureBuffer)
        {
          if (futureBuffer.hasError())
          {
            fail(futureBuffer.error());
            clearLocalFile();
            return;
          }
          if (promise.isCancelRequested())
          {
            clearLocalFile();
            cancel();
            return;
          }

          if (bytesWritten < fileSize)
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

      void clearLocalFile()
      {
        localFile.close();
        boost::filesystem::remove(localPath);
      }

      boost::filesystem::ofstream localFile;
      std::streamsize bytesWritten = 0;
      const qi::Path localPath;
    };

  };

}

#endif
