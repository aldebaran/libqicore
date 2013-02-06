#ifndef LOGGER_SERVICE_HPP
#define LOGGER_SERVICE_HPP
#include <qitype/signal.hpp>
#include <qitype/property.hpp>
#include <boost/shared_ptr.hpp>
#include <src/logger.hpp>
class Logger;
typedef boost::shared_ptr<Logger> LoggerPtr;
class LoggerProxy;
typedef boost::shared_ptr<LoggerProxy> LoggerProxyPtr;
class LogProviderProxy;
typedef boost::shared_ptr<LogProviderProxy > LogProviderProxyPtr;
/** Registers to a local or remote Logger service
*  Sends local logger message to it
*  Honors commands from it to configure local logger verbosity.
*  @threadSafe
*/
class LogProvider
{
public:
  LogProvider(LoggerProxyPtr logger);
  ~LogProvider();
  void setVerbosity(qi::log::LogLevel level);
  void setCategory(const std::string& cat, qi::log::LogLevel level);
  void clearAndSet(const std::vector<std::pair<std::string, qi::log::LogLevel> >& data);
private:
  void log(qi::log::LogLevel level, qi::os::timeval tv, const char* file, const char* function, const char* category, const char* message, int line);
  std::set<std::string> _setCategories;
  LoggerProxyPtr  _logger;
  qi::log::Subscriber _subscriber;
};

QI_TYPE_NOT_CLONABLE(LogProvider);

typedef boost::shared_ptr<LogProvider> LogProviderPtr;

/// implemented service side, client have proxies. @threadSafe.
class LogListener
{
public:
  explicit LogListener(Logger& l);
  void setVerbosity(qi::log::LogLevel level);
  void setCategory(const std::string& cat, qi::log::LogLevel level);
  /// Remove all filters set by setCategory
  void clearFilters();
  qi::Signal<void (Message)> onMessage;
  qi::Property<qi::log::LogLevel> verbosity;
private:
  void log(const Message& m);
  typedef std::map<std::string, qi::log::LogLevel> FilterMap;
  FilterMap _filters;
  Logger& _logger;
  friend class Logger;
  friend bool set_verbosity(LogListener* ll,  qi::log::LogLevel& level,
  const qi::log::LogLevel& newvalue);
};

QI_TYPE_NOT_CLONABLE(LogListener);


typedef boost::shared_ptr<LogListener> LogListenerPtr;

/// @threadSafe
class Logger
{
public:
  Logger();
  void log(const Message& msg);
  LogListenerPtr getListener();
  void addProvider(LogProviderProxyPtr provider);
private:
  void recomputeCategories();
  void recomputeVerbosities(qi::log::LogLevel from, qi::log::LogLevel to);
  qi::log::LogLevel _maxLevel;
  std::vector<std::pair<std::string, qi::log::LogLevel> > _filters;
  std::vector<LogProviderProxyPtr> _providers;
  /// FIXME keep a week-ptr
  std::vector<LogListenerPtr> _listeners;
  friend class LogListener;
  friend bool set_verbosity(LogListener* ll,  qi::log::LogLevel& level,
  const qi::log::LogLevel& newvalue);
};
#endif

