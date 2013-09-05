
#include <qitype/anyobject.hpp>
#include <qitype/objecttypebuilder.hpp>
#include <qitype/objectfactory.hpp>

#include <qicore/logmessage.hpp>
#include <qicore/logprovider.hpp>




static int LogProviderinit()
{
  qi::ObjectTypeBuilder<LogProvider> builder;
  builder.setThreadingModel(qi::ObjectThreadingModel_MultiThread);
  builder.advertiseMethod("setVerbosity", &LogProvider::setVerbosity, qi::MetaCallType_Auto);
  builder.advertiseMethod("setCategory", &LogProvider::setCategory, qi::MetaCallType_Auto);
  builder.advertiseMethod("clearAndSet", &LogProvider::clearAndSet, qi::MetaCallType_Auto);

  builder.registerType();
  return 0;
}
static int _init_LogProvider = LogProviderinit();


