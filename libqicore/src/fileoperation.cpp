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

  FileOperationPtr prepareCopyToLocal(FilePtr file, const Path& localPath)
  {
    return boost::make_shared<FileCopyToLocal>(std::move(file), localPath);
  }

  void _qiregisterFileOperation()
  {
    ::qi::ObjectTypeBuilder<FileOperation> builder;
    QI_OBJECT_BUILDER_ADVERTISE(builder, FileOperation, start);
    QI_OBJECT_BUILDER_ADVERTISE(builder, FileOperation, startStandAlone);
    QI_OBJECT_BUILDER_ADVERTISE(builder, FileOperation, notifier);
    QI_OBJECT_BUILDER_ADVERTISE(builder, FileOperation, isValid);

    builder.registerType();
  }

  void registerFileOperations(qi::ModuleBuilder& mb)
  {
    mb.advertiseMethod("copyToLocal", &copyToLocal);
    mb.advertiseMethod("prepareCopyToLocal", &prepareCopyToLocal);
  }

}
