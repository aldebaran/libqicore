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
  /** Base type for file operation exposing information about its progress state.
      Owns a task that will execute the operation.
      Exposes a ProgressNotifier, associated to the operation.

      As this is a move-able type, the task ownership can be moved and it is possible
      to have a file operation object not owning a task. In this case,
      no member call can succeed except re-assigning a valid file operation.

      @includename{qicore/file.hpp}
  **/
  class FileOperation
  {
  public:
    /** Destructor.
        Cancel the task if it is still owned by this object and is still running.
    **/
    virtual ~FileOperation()
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

    /** Starts the operation's task.

        Throws a std::runtime_error if one of these conditions is true:
         - start() has already been called before at least once;
         - startStandAlone() has already been called before at least once;
         - this object does not own the operation's task anymore;

        @return A future corresponding to the end of the operation.
    **/
    qi::Future<void> start()
    {
      acquireTask();
      return _task->run();
    }

    /** Starts the operation's task after losing its ownership.
        This is useful when you want to launch the task but not keep track
        of its lifetime.

        Throws a std::runtime_error if one of these conditions is true:
         - start() has already been called before at least once;
         - startStandAlone() has already been called before at least once;
         - this object does not own the operation's task anymore;

        @return A future corresponding to the end of the operation.
    **/
    qi::Future<void> startStandAlone()
    {
      acquireTask();
      boost::shared_ptr<Task> sharedTask{ std::move(_task) };
      auto future = sharedTask->run();
      future.connect([sharedTask](Future<void>&){}); // keep the task alive until it ends
      return future;
    }

    /// Call operator: calls start()
    auto operator()() -> decltype(start()) { return start(); }

    /** @returns A progress notifier associated to the operation if the operation's task is owned,
                 null otherwise.
    **/
    ProgressNotifierPtr notifier() const { return _task ? _task->localNotifier : ProgressNotifierPtr{}; }

    /// @returns True if this object owns the operation's task, false otherwise.
    bool isValid() const { return _task ? true : false; }

    /// @returns True if this object owns the operation's task, false otherwise.
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
        localNotifier->reset();
        remoteNotifier->reset();
        localNotifier->notifyRunning();
        remoteNotifier->notifyRunning();
        start();
        return promise.future();
      }

      void finish()
      {
        promise.setValue(0);
        localNotifier->notifyFinished();
        remoteNotifier->notifyFinished();
      }

      void fail(const std::string& errorMessage)
      {
        promise.setError(errorMessage);
        localNotifier->notifyFailed();
        remoteNotifier->notifyFailed();
      }

      void cancel()
      {
        promise.setCanceled();
        localNotifier->notifyCanceled();
        remoteNotifier->notifyCanceled();
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
        throw std::runtime_error{ "Called FileOperation::start() more than once!" };
      }
    }
  };

  /** Copies a potentially remote file to the local file system. */
  class FileCopyToLocal
    : public FileOperation
  {
  public:
    /** Constructor.
        @param file        Access to a potentially remote file to copy to the local file system.
        @param localPath   Local file system location where the specified file will be copied.
                           No file or directory should be located at this path otherwise
                           the operation will fail.
    **/
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
        localNotifier->notifyProgressed(progress);
        remoteNotifier->notifyProgressed(progress);
      }

      void fetchData()
      {
        static const size_t ARBITRARY_BYTES_TO_READ_PER_CYCLE = 512 * 1024;
        auto myself = shared_from_this();
        sourceFile.async<Buffer>("read", bytesWritten, ARBITRARY_BYTES_TO_READ_PER_CYCLE)
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

  /** Copy an open local or remote file to a local file system location.
  *   @param file         Source file to copy.
  *   @param localPath    Local file system location where the specified file will be copied.
  *                       No file or directory should be located at this path otherwise
  *                       the operation will fail.
  *   @return A synchronous future associated with the operation.
  **/
  QICORE_API FutureSync<void> copyToLocal(FilePtr file, const Path& localPath);

}

#endif
