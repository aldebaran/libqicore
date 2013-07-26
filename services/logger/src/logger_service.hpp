#ifndef LOGGER_SERVICE_HPP
# define LOGGER_SERVICE_HPP

# include <qitype/signal.hpp>
# include <qitype/property.hpp>
# include <boost/shared_ptr.hpp>
# include <src/logger.hpp>

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
  LoggerProxyPtr        _logger;
  qi::log::Subscriber   _subscriber;
};

QI_TYPE_NOT_CLONABLE(LogProvider);
typedef boost::shared_ptr<LogProvider> LogProviderPtr;


/// implemented service side, client have proxies. @threadSafe.
class LogListener
{
public:
  explicit LogListener(Logger& l);

  void setVerbosity(qi::LogLevel level);
  void setCategory(const std::string& cat,
                   qi::LogLevel level);

  /// Remove all filters set by setCategory
  void clearFilters();

public:
  qi::Signal<Message> onMessage;
  qi::Property<qi::LogLevel> verbosity;

private:
  typedef std::map<std::string, qi::LogLevel> FilterMap;
  FilterMap _filters;
  Logger&   _logger;

private:
  friend class Logger;
  friend bool set_verbosity(LogListener* ll,
                            qi::LogLevel& level,
                            const qi::LogLevel& newvalue);

  void log(const Message& m);
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
  void recomputeVerbosities(qi::LogLevel from, qi::LogLevel to);

private:
  qi::LogLevel _maxLevel;
  std::vector<std::pair<std::string, qi::LogLevel> > _filters;

  /// FIXME keep a week-ptr
  std::vector<LogListenerPtr>      _listeners;
  std::vector<LogProviderProxyPtr> _providers;

private:
  friend class LogListener;
  friend bool set_verbosity(LogListener* ll,
                            qi::LogLevel& level,
                            const qi::LogLevel& newvalue);
};

#endif // !LOGGER_SERVICE_HPP
