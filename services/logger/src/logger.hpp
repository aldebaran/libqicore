#ifndef QICORE_LOGGER_HPP
#define QICORE_LOGGER_HPP

#include <qi/log.hpp>
#include <qitype/type.hpp>
#include <boost/shared_ptr.hpp>

QI_TYPE_ENUM_REGISTER(qi::log::LogLevel)

struct Message
{
  std::string source;
  qi::log::LogLevel level;
  qi::os::timeval timestamp;
  std::string category;
  std::string location;
  std::string message;
};

QI_TYPE_STRUCT_REGISTER(::Message, source, level, timestamp, category, location, message);

class LoggerProxy;
typedef boost::shared_ptr<LoggerProxy> LoggerProxyPtr;

// Register local logger to service
void registerToLogger(LoggerProxyPtr logger);



#endif
