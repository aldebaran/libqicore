/*
** Author(s):
**  - Herve Cuche <hcuche@aldebaran-robotics.com>
**  - Matthieu Nottale <mnottale@aldebaran-robotics.com>
**
** Copyright (C) 2013 Aldebaran Robotics
*/

#ifndef LOGPROVIDER_HPP_
# define LOGPROVIDER_HPP_

# include <boost/shared_ptr.hpp>

# include <qi/log.hpp>
# include <qi/macro.hpp>

# include <logger/logger.hpp>
# include <services/logger/loggermanager_proxy.hpp>
# include <services/logger/loglistener_proxy.hpp>

# include <set>
# include <string>
# include <utility> // std::pair
# include <vector>

class LogProvider;
typedef boost::shared_ptr<LogProvider> LogProviderPtr;

// Register local provider to service
QI_API void registerToLogger(LoggerManagerProxyPtr logger);

/** Registers to a local or remote Logger service
 *  Sends local logger message to it
 *  Honors commands from it to configure local logger verbosity.
 *  @threadSafe
 */
class QI_API LogProvider
{
 public:
  LogProvider(LoggerManagerProxyPtr logger);
  ~LogProvider();

  void setVerbosity(qi::LogLevel level);
  void setCategory(const std::string& cat,
                   qi::LogLevel level);
  void clearAndSet(const std::vector<std::pair<std::string, qi::LogLevel> >& data);

 private:
  void log(qi::LogLevel level,
           qi::os::timeval tv,
           const char* category,
           const char* message,
           const char* file,
           const char* function,
           int line);

 private:
  std::set<std::string> _setCategories;
  LoggerManagerProxyPtr _logger;
  qi::log::Subscriber   _subscriber;
};
QI_TYPE_NOT_CLONABLE(LogProvider);

#endif // !LOGPROVIDER_HPP_
