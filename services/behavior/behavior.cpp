#include <fstream>

#include <qitype/objectfactory.hpp>
#include <qimessaging/session.hpp>
#include <boost/foreach.hpp>
#include <qicore/behavior.hpp>

#define foreach BOOST_FOREACH

using qi::AnyObject;
using qi::Signature;
using qi::SignalLink;

qiLogCategory("service.behavior");

template<typename T> inline T find0Ptr(std::map<std::string, T>& m, const std::string& key)
{
  typename std::map<std::string, T>::iterator it = m.find(key);
  if (it == m.end())
    return T();
  else
    return it->second;
}

class Behavior
{
public:
  typedef std::map<std::string, AnyObject> ObjectMap;
  /* Two transision modes:
   * - Direct: link is a link to a connect(source, target, targetMethod)
   * - Intercept: debug mode, link is a connect(source, &Behavior::transition)
   */
  struct TransitionPtr
  {
    AnyObject source;
    AnyObject target;
    SignalLink link;
    std::string property; // target property name
    unsigned int targetMethod; // or target method id
    bool debug;
  };
  typedef std::map<std::string, TransitionPtr> TransitionMap;

public:
  Behavior(): _session(new qi::Session) {}
  ~Behavior() { delete _session;}
  AnyObject makeObject(const std::string& model, const std::string& factory, const qi::BehaviorModel::ParameterMap& params);
  void loadObjects(bool debugmode);
  void unloadObjects();
  void setTransitions(bool debugmode, qi::MetaCallType type = qi::MetaCallType_Auto);
  void removeTransitions();
  void loadFile(const std::string& path);
  void loadString(const std::string& data);
  void connect(const std::string& url);
  ObjectPtr object(const std::string& name);
  qi::AnyValue call(const std::string& objUid, const std::string& fun, std::vector<qi::AnyValue> args);

  /// Triggered when a transition occurrs, if transitions were set in debug mode.
  qi::Signal<const std::string&, qi::AnyValue> onTransition;
  /// Triggered when one of the tasks enter/leave an error state
  qi::Signal<const std::string&, const std::string&> onTaskError;
  /// Triggered when one of the tasks's running property changes state
  qi::Signal<const std::string&, bool> onTaskRunning;

public:
  void setModel(const qi::BehaviorModel &model) { _model = model; }
  qi::Session&       session()     { return *_session;};
  qi::BehaviorModel& model()       { return _model; };
  ObjectMap&         objects()     { return _objects; };
  TransitionMap&     transitions() { return _transitions; };
  //TODO: DOES NOT BUILD WITH BOTH const and non const
  //const BehaviorModel&   model() const       { return _model; };
  //const ObjectMap&       objects() const     { return _objects; };
  //const TransitionMap&   transitions() const { return _transitions; };

private:
  void transition(qi::AnyValue argument, const std::string& transitionId);

private:
  typedef qi::SignalBase::Link Link;
  // hooks on running/error
  typedef std::map<std::string, std::pair<Link, Link> > ObjectLinks;
  ObjectLinks       _objectLinks;
  qi::BehaviorModel _model;
  ObjectMap         _objects;
  TransitionMap     _transitions;
  qi::Session*      _session;
};

QI_REGISTER_OBJECT(Behavior, loadObjects, unloadObjects, setTransitions, removeTransitions, loadFile, loadString, connect, call, onTransition, model, objects, transitions, setModel, onTaskRunning, onTaskError);
QI_REGISTER_OBJECT_FACTORY_BUILDER(Behavior);


qi::AnyValue Behavior::call(const std::string& objUid, const std::string& fun,
  std::vector<qi::AnyValue> args)
{
  qiLogDebug() << "Calling " << objUid << '.' << fun;
  using qi::AnyValue;
  AnyObject o = find0Ptr(_objects, objUid);
  if (!o)
    throw std::runtime_error("Object not found: " + objUid);
  // Don't want to reipmelemnt signature resolution? No problem!
  switch(args.size())
  {
  case 0:
    return o->call<AnyValue>(fun);
    break;
  case 1:
    return o->call<AnyValue>(fun, args[0]);
    break;
  case 2:
    return o->call<AnyValue>(fun, args[0], args[1]);
    break;
  case 3:
    return o->call<AnyValue>(fun, args[0], args[1], args[2]);
    break;
  case 4:
    return o->call<AnyValue>(fun, args[0], args[1], args[2], args[3]);
    break;
  default:
    throw std::runtime_error("argument count not implemented");
    break;
  }
  throw std::runtime_error("argument count not implemented");
}

void Behavior::connect(const std::string& url)
{
  session().connect(url);
}

static void onTaskErrorBounce(Behavior* b, const std::string& task, const std::string& err)
{
  qiLogDebug() << "onTaskError " << task << " " << err;
  b->onTaskError(task, err);
}

static void onTaskRunningBounce(Behavior* b, const std::string& task, bool state)
{
  qiLogDebug() << "onTaskRunning " << task << " " << state;
  b->onTaskRunning(task, state);
}

void Behavior::loadObjects(bool debugmode)
{
  if (!_objects.empty())
    throw std::runtime_error("Objects already present");
  foreach(qi::BehaviorModel::NodeMap::value_type& n, _model.nodes)
  {
    qiLogDebug() << "loading " << n.first << " from " <<n.second.factory;
    AnyObject o = makeObject(n.second.interface, n.second.factory, n.second.parameters);
    _objects[n.first] = o;
    if (debugmode)
    {
      qi::MetaObject mo = o->metaObject();
      int pRunningId = mo.propertyId("running");
      int pErrorId = mo.propertyId("error");
      if (pRunningId!= -1 && pErrorId != -1)
      {
        qiLogDebug() << "Detected a Task, hooking signals...";
        SignalLink lRunning = o->connect("running",
          (boost::function<void(bool)>)boost::bind(
            &onTaskRunningBounce, this, n.first, _1));

        SignalLink lError = o->connect("error",
          (boost::function<void(const std::string&)>)boost::bind(
            onTaskErrorBounce, this, n.first, _1));
        _objectLinks[n.first] = std::make_pair(lRunning, lError);
        if (lRunning == qi::SignalBase::invalidSignalLink
          || lError == qi::SignalBase::invalidSignalLink)
          qiLogError() << "Failed to hook Task signals for " << n.first;
      }
    }
  }
  qiLogDebug() << "loadObjects finished";
}

void Behavior::unloadObjects()
{
  if (!_transitions.empty())
    throw std::runtime_error("Refusing to unload with transitions presents");
  // unhook Task signals
  for (ObjectLinks::iterator it = _objectLinks.begin(); it != _objectLinks.end(); ++it)
  {
    AnyObject& obj = _objects[it->first];
    if (!obj)
    {
      qiLogError() << "Expected object in map for " << it->first;
      continue;
    }
    obj->disconnect(it->second.first);
    obj->disconnect(it->second.second);
  }
  _objectLinks.clear();
  _objects.clear();
}

void Behavior::setTransitions(bool debugmode, qi::MetaCallType type)
{
  qiLogDebug() << "setTransitions";
  if (!_transitions.empty())
    throw std::runtime_error("Transitions already present");
  foreach(qi::BehaviorModel::TransitionMap::value_type& vt, _model.transitions)
  {
    qi::BehaviorModel::Transition& t = vt.second;
    AnyObject src = find0Ptr(_objects, t.src.first);
    AnyObject dst = find0Ptr(_objects, t.dst.first);
    if (!src)
      throw std::runtime_error("No object " + t.src.first);
    if (!dst)
      throw std::runtime_error("No object " + t.dst.first);
    std::vector<qi::MetaSignal> srcSignals = src->metaObject().findSignal(t.src.second);
    std::vector<qi::MetaMethod> dstMethods = dst->metaObject().findMethod(t.dst.second);
    // also lookup for a matching property
    int propId = dst->metaObject().propertyId(t.dst.second);
    const qi::MetaProperty* prop = dst->metaObject().property(propId);
    // fixme: handle properties as target
    if (srcSignals.empty())
      throw std::runtime_error("No signal " + t.src.second);
    if (dstMethods.empty() && !prop)
      throw std::runtime_error("No method or property " + t.dst.second);
    // find a compatible pair
    std::pair<unsigned, unsigned> best;
    static const unsigned PROP_MATCH = 0xFFFFFFFF;
    float bestScore = 0;
    for (unsigned s=0; s<srcSignals.size(); ++s)
    {
      Signature sigS = srcSignals[s].parametersSignature();
      for (unsigned d=0; d<dstMethods.size(); ++d)
      {
        Signature sigD = dstMethods[d].parametersSignature();
        float score = sigS.isConvertibleTo(sigD);
        qiLogDebugF("scoring %s -> %s : %s", sigS.toString(), sigD.toString(), score);
        if (score > bestScore)
        {
          bestScore = score;
          best = std::make_pair(s, d);
        }
      }
      if (prop)
      {
        std::string sigD = "(" + prop->signature().toString() + ")";
        float score = sigS.isConvertibleTo(Signature(sigD));
        qiLogDebugF("scoring %s -> %s : %s", sigS.toString(), sigD, score);
        if (score > bestScore)
        {
          bestScore = score;
          best = std::make_pair(s, PROP_MATCH);
        }
      }
    }
    if (!bestScore)
      throw std::runtime_error("Could not match " + t.toString());
    qiLogDebug() << "Best match " << best.second;
    TransitionPtr& ptr = _transitions[t.uid];
    ptr.debug = debugmode;
    ptr.source = src;
    ptr.target = dst;
    if (best.second == PROP_MATCH)
      ptr.property = t.dst.second;
    else
      ptr.targetMethod = dstMethods[best.second].uid();
    // If target is a property, we need to use our bouncer to invoke setProperty
    if (debugmode || best.second == PROP_MATCH || t.filter.isValue())
      ptr.link = src->connect(srcSignals[best.first].uid(),
        qi::SignalSubscriber(qi::AnyFunction::from((boost::function<void(qi::AnyValue)>) boost::bind(&Behavior::transition, this, _1, t.uid)),type));
    else
      ptr.link = src->connect(srcSignals[best.first].uid(), qi::SignalSubscriber(dst, ptr.targetMethod));
  }
}

void Behavior::removeTransitions()
{
  foreach(TransitionMap::value_type& t, _transitions)
  {
    t.second.source->disconnect(t.second.link);
  }
  _transitions.clear();
}

AnyObject Behavior::makeObject(const std::string& model, const std::string& factory,
  const qi::BehaviorModel::ParameterMap& parameters)
{
  size_t p = factory.find_first_of(':');
  if (p == factory.npos)
  {
    // method not set, heuristics mode
    p = factory.find_first_of('.');
    std::string object = factory;
    std::string method;
    if (p != factory.npos)
    {
      object = factory.substr(0, p);
      method = factory.substr(p+1);
    }
    AnyObject s;
    try
    {
      s = session().service(object);
    }
    catch(...)
    {
    }
    if (!s)
    {
      // not a service, try local factory from loaded shared objects
      // FIXME: auto-load some .so
      s = qi::createObject(object);
      if (!s)
        throw std::runtime_error(object +" is neither a service nor available through factory");
    }
    // Heuristic: autodetect factory and invoke create
    // Do we realy want that?
    if (method.empty() && !s->metaObject().findMethod("create").empty())
      method = "create";
    if (!method.empty())
    {
      s = s->call<AnyObject>(method);
    }
    for (qi::BehaviorModel::ParameterMap::const_iterator it = parameters.begin(); it != parameters.end(); ++it)
    {
      s->setProperty(it->first, it->second);
    }
    return s;
  }
  else
    throw std::runtime_error("Not implemented");
}

void Behavior::loadFile(const std::string& path)
{
  if (!_objects.empty())
    throw std::runtime_error("Objects present, unload them before loading a new file");
  _model.clear();
  std::ifstream ifs(path.c_str());
  _model.load(ifs);
}

void Behavior::loadString(const std::string& path)
{
  if (!_objects.empty())
    throw std::runtime_error("Objects present, unload them before loading a new file");
  _model.clear();
  std::stringstream is;
  is.str(path);
  _model.load(is);
}

void Behavior::transition(qi::AnyValue arg, const std::string& transId)
{
  TransitionPtr t = _transitions[transId];
  qi::BehaviorModel::Transition& modelTransition = _model.transitions[transId];
  if (modelTransition.filter.isValue())
  {
    qiLogDebug() << "Filtering " << qi::encodeJSON(arg) <<" with " << qi::encodeJSON(modelTransition.filter);
    bool match = !(arg < modelTransition.filter) && !(modelTransition.filter < arg);
    if (!match)
      return;
  }
  if (t.debug)
    onTransition(transId, arg);

  qiLogDebug() << "Intercepted transition " << transId << " to " << t.targetMethod;
  if (!t.property.empty())
    t.target->setProperty(t.property, arg);
  else
  {
    qi::GenericFunctionParameters args;
    args.push_back(qi::AnyReference(arg.type, arg.value));
    t.target->metaPost(t.targetMethod, args);
  }
}

qi::ObjectPtr Behavior::object(const std::string& s)
{
  qi::ObjectPtr res = _objects[s];
  qiLogDebug() << "transmiting object " << s <<" : " << res;
  return res;
}

