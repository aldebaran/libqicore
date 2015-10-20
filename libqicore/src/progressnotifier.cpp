/*
**  Copyright (C) 2012 Aldebaran Robotics
**  See COPYING for the license
*/

#include <qicore/file.hpp>
#include <qi/anymodule.hpp>

namespace qi
{
  class ProgressNotifierImpl
    : public ProgressNotifier
  {
  public:
    explicit ProgressNotifierImpl(Future<void> operationFuture)
      : _opFuture(std::move(operationFuture))
    {
      this->status.set(ProgressNotifier::Status_Idle);
    }

    void reset() override
    {
      this->status.set(ProgressNotifier::Status_Idle);
      this->progress.set(0.0);
    }

    void notifyRunning() override
    {
      if (this->status.get() != ProgressNotifier::Status_Idle)
        qiLogError("qicore.file.progressnotifierbase")
        << "ProgressNotifier must be Idle to be allowed to switch to Running status.";

      this->status.set(ProgressNotifier::Status_Running);
    }

    void notifyFinished() override
    {
      if (!isRunning())
        qiLogError("qicore.file.progressnotifierbase")
        << "ProgressNotifier must be Running to be allowed to switch to Finished status.";

      this->status.set(ProgressNotifier::Status_Finished);
    }

    void notifyCanceled() override
    {
      if (!isRunning())
        qiLogError("qicore.file.progressnotifierbase")
        << "ProgressNotifier must be Running to be allowed to switch to Canceled status.";
      this->status.set(ProgressNotifier::Status_Canceled);
    }

    void notifyFailed() override
    {
      if (!isRunning())
        qiLogError("qicore.file.progressnotifierbase")
        << "ProgressNotifier must be Running to be allowed to switch to Failed status.";
      this->status.set(ProgressNotifier::Status_Failed);
    }

    void notifyProgressed(double newProgress) override
    {
      if (!isRunning())
        qiLogError("qicore.file.progressnotifierbase")
        << "ProgressNotifier must be Running to be allowed to notify any progress.";
      this->progress.set(newProgress);
    }

    bool isRunning() const override
    {
      return this->status.get() == ProgressNotifier::Status_Running;
    }

    Future<void> waitForFinished() override
    {
      return _opFuture;
    }

    Future<void> _opFuture;
  };

QI_REGISTER_OBJECT(ProgressNotifier,
                   notifyRunning,
                   notifyFinished,
                   notifyCanceled,
                   notifyFailed,
                   notifyProgressed,
                   waitForFinished,
                   isRunning,
                   reset);
QI_REGISTER_IMPLEMENTATION(ProgressNotifier, ProgressNotifierImpl);

ProgressNotifierPtr createProgressNotifier(Future<void> operationFuture)
{
  return boost::make_shared<ProgressNotifierImpl>(std::move(operationFuture));
}
}
