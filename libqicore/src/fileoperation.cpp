/*
**  Copyright (C) 2012 Aldebaran Robotics
**  See COPYING for the license
*/

#include <qicore/file.hpp>

namespace qi
{
  FutureSync<void> copyToLocal(FilePtr file, const Path& localPath)
  {
    FileCopyToLocal task{ std::move(file), localPath };
    return task.startStandAlone();
  }
}
