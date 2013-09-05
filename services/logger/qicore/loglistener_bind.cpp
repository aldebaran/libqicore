
#include <qitype/anyobject.hpp>
#include <qitype/objecttypebuilder.hpp>
#include <qitype/objectfactory.hpp>

#include <qicore/logmessage.hpp>
#include <src/logmanager.hpp>




static int LogListenerinit()
{
  qi::ObjectTypeBuilder<LogListener> builder;
  builder.setThreadingModel(qi::ObjectThreadingModel_MultiThread);
  builder.advertiseMethod("setVerbosity", &LogListener::setVerbosity, qi::MetaCallType_Auto);
  builder.advertiseMethod("setCategory", &LogListener::setCategory, qi::MetaCallType_Auto);
  builder.advertiseMethod("clearFilters", &LogListener::clearFilters, qi::MetaCallType_Auto);
  builder.advertise("onLogMessage", &LogListener::onLogMessage);
  builder.advertise("verbosity", &LogListener::verbosity);

  builder.registerType();
  return 0;
}
static int _init_LogListener = LogListenerinit();


