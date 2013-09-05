
#include <qitype/anyobject.hpp>
#include <qitype/objecttypebuilder.hpp>
#include <qitype/objectfactory.hpp>

#include <qicore/logmessage.hpp>
#include <src/logmanager.hpp>



namespace qi
{


static int LogManagerinit()
{
  qi::ObjectTypeBuilder<LogManager> builder;
  builder.setThreadingModel(qi::ObjectThreadingModel_MultiThread);
  builder.advertiseMethod("log", &LogManager::log, qi::MetaCallType_Auto);
  builder.advertiseMethod("getListener", &LogManager::getListener, qi::MetaCallType_Auto);
  builder.advertiseMethod("addProvider", &LogManager::addProvider, qi::MetaCallType_Auto);

  builder.registerType();
  return 0;
}
static int _init_LogManager = LogManagerinit();
QI_REGISTER_OBJECT_FACTORY_CONSTRUCTOR(LogManager);

} // !qi
