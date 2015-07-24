/*
**  Copyright (C) 2012 Aldebaran Robotics
**  See COPYING for the license
*/
#ifndef QI_CORE_PROGRESS_NOTIFIER_IMPL_HPP
#define QI_CORE_PROGRESS_NOTIFIER_IMPL_HPP
#pragma once

#include <qicore/file.hpp>
#include <qi/anymodule.hpp>

namespace qi
{
template <class T>
class ProgressNotifierBaseImpl : public T
{
public:
  ProgressNotifierBaseImpl()
  {
    this->status.set(ProgressNotifier::Status_Idle);
  }

  void _reset()
  {
    this->status.set(ProgressNotifier::Status_Idle);
    this->progress.set(0.0);
  }

  void _notifyRunning()
  {
    if (this->status.get() != ProgressNotifier::Status_Idle)
      qiLogError("qicore.file.progressnotifierbase")
          << "ProgressNotifier must be Idle to be allowed to switch to Running status.";

    this->status.set(ProgressNotifier::Status_Running);
  }

  void _notifyFinished()
  {
    if (!isRunning())
      qiLogError("qicore.file.progressnotifierbase")
          << "ProgressNotifier must be Running to be allowed to switch to Finished status.";

    this->status.set(ProgressNotifier::Status_Finished);
  }

  void _notifyCancelled()
  {
    if (!isRunning())
      qiLogError("qicore.file.progressnotifierbase")
          << "ProgressNotifier must be Running to be allowed to switch to Cancelled status.";
    this->status.set(ProgressNotifier::Status_Cancelled);
  }

  void _notifyFailed()
  {
    if (!isRunning())
      qiLogError("qicore.file.progressnotifierbase")
          << "ProgressNotifier must be Running to be allowed to switch to Failed status.";
    this->status.set(ProgressNotifier::Status_Failed);
  }

  void _notifyProgressed(double newProgress)
  {
    if (!isRunning())
      qiLogError("qicore.file.progressnotifierbase")
          << "ProgressNotifier must be Running to be allowed to notify any progress.";
    this->progress.set(newProgress);
  }

  bool isRunning() const
  {
    return this->status.get() == ProgressNotifier::Status_Running;
  }

  Future<void> waitForFinished()
  {
    return _opFuture;
  }

  Future<void> _opFuture;
};
}

#endif
