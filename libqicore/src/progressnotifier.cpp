/*
**  Copyright (C) 2012 Aldebaran Robotics
**  See COPYING for the license
*/

#include "progressnotifier_p.hpp"

namespace qi
{
typedef ProgressNotifierBaseImpl<ProgressNotifier> ProgressNotifierImpl;

QI_REGISTER_OBJECT(ProgressNotifier,
                   _notifyRunning,
                   _notifyFinished,
                   _notifyCancelled,
                   _notifyFailed,
                   _notifyProgressed,
                   waitForFinished,
                   isRunning,
                   _reset);
QI_REGISTER_IMPLEMENTATION(ProgressNotifier, ProgressNotifierImpl);

ProgressNotifierPtr createProgressNotifier()
{
  return boost::make_shared<ProgressNotifierImpl>();
}
}
