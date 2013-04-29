#include <fstream>

#include <qitype/objectfactory.hpp>
#include <qimessaging/session.hpp>
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH

using qi::ObjectPtr;
using qi::Signature;
using qi::Link;

qiLogCategory("service.behavior");

template<typename T> inline T find0Ptr(std::map<std::string, T>& m, const std::string& key)
{
  typename std::map<std::string, T>::iterator it = m.find(key);
  if (it == m.end())
    return T();
  else
    return it->second;
}

inline std::pair<std::string, std::string> splitString2(const std::string& s, char sep)
{
  size_t p = s.find_first_of(sep);
  if (p == s.npos)
    throw std::runtime_error("separator not found");
  return std::make_pair(s.substr(0, p), s.substr(p+1));
}

struct BehaviorModel
{
  typedef std::pair<std::string, std::string> Slot; // objectUid.methodName
  struct Node
  {
    std::string uid;
    std::string interface;
    std::string factory;
  };
  struct Transition
  {
    std::string uid;
    Slot src; // uid.slot
    Slot dst; // uid.slot
    std::string toString()
    {
      return src.first + '.' + src.second + " -> " + dst.first + '.' + dst.second;
    }
  };

  typedef std::map<std::string, Node> NodeMap;
  typedef std::map<std::string, Transition> TransitionMap;
  NodeMap       nodes;
  TransitionMap transitions;

  void load(std::istream& is);
  void clear();
};

class Behavior
{
public:
  Behavior(): _session(new qi::Session) {}
  ~Behavior() { delete _session;}
  ObjectPtr makeObject(const std::string& model, const std::string& factory);
  void loadObjects();
  void unloadObjects();
  void setTransitions(bool debugmode);
  void removeTransitions();
  void loadFile(const std::string& path);
  void loadString(const std::string& data);
  void connect(const std::string& url);
  qi::GenericValue call(const std::string& objUid, const std::string& fun, std::vector<qi::GenericValue> args);
  qi::Session& session() { return *_session;};
  /// Triggered when a transition occurrs, if transitions were set in debug mode.
  qi::Signal<void (const std::string&, qi::GenericValue)> onTransition;
private:
  void transition(qi::GenericValue argument, const std::string& transitionId);
  BehaviorModel  _model;
  typedef std::map<std::string, ObjectPtr> ObjectMap;
  ObjectMap _objects;
  /* Two transision modes:
   * - Direct: link is a link to a connect(source, target, targetMethod)
   * - Intercept: debug mode, link is a connect(source, &Behavior::transition)
   */
  struct TransitionPtr
  {
    ObjectPtr source;
    ObjectPtr target;
    Link      link;
    std::string property; // target property name
    unsigned int targetMethod; // or target method id
    bool debug;
  };
  typedef std::map<std::string, TransitionPtr> TransitionMap;
  TransitionMap _transitions;
  qi::Session* _session;
};

QI_REGISTER_OBJECT(Behavior, loadObjects, unloadObjects, setTransitions, removeTransitions, loadFile, loadString, connect, call, onTransition);
QI_REGISTER_OBJECT_FACTORY_BUILDER(Behavior);


qi::GenericValue Behavior::call(const std::string& objUid, const std::string& fun,
  std::vector<qi::GenericValue> args)
{
  qiLogDebug() << "Calling " << objUid << '.' << fun;
  using qi::GenericValue;
  ObjectPtr o = find0Ptr(_objects, objUid);
  if (!o)
    throw std::runtime_error("Object not found: " + objUid);
  // Don't want to reipmelemnt signature resolution? No problem!
  switch(args.size())
  {
  case 0:
    return o->call<GenericValue>(fun);
    break;
  case 1:
    return o->call<GenericValue>(fun, args[0]);
    break;
  case 2:
    return o->call<GenericValue>(fun, args[0], args[1]);
    break;
  case 3:
    return o->call<GenericValue>(fun, args[0], args[1], args[2]);
    break;
  case 4:
    return o->call<GenericValue>(fun, args[0], args[1], args[2], args[3]);
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

void Behavior::loadObjects()
{
  if (!_objects.empty())
    throw std::runtime_error("Objects already present");
  foreach(BehaviorModel::NodeMap::value_type& n, _model.nodes)
  {
    qiLogDebug() << "loading " << n.first << " from " <<n.second.factory;
    ObjectPtr o = makeObject(n.second.interface, n.second.factory);
    _objects[n.first] = o;
  }
  qiLogDebug() << "loadObjects finished";
}

void Behavior::unloadObjects()
{
  if (!_transitions.empty())
    throw std::runtime_error("Refusing to unload with transitions presents");
  _objects.clear();
}

void Behavior::setTransitions(bool debugmode)
{
  qiLogDebug() << "setTransitions";
  if (!_transitions.empty())
    throw std::runtime_error("Transitions already present");
  foreach(BehaviorModel::TransitionMap::value_type& vt, _model.transitions)
  {
    BehaviorModel::Transition& t = vt.second;
    ObjectPtr src = find0Ptr(_objects, t.src.first);
    ObjectPtr dst = find0Ptr(_objects, t.dst.first);
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
      std::string sigS = qi::signatureSplit(srcSignals[s].signature())[2];
      for (unsigned d=0; d<dstMethods.size(); ++d)
      {
        std::string sigD = dstMethods[d].parametersSignature();
        float score = Signature(sigS).isConvertibleTo(Signature(sigD));
        qiLogDebugF("scoring %s -> %s : %s", sigS, sigD, score);
        if (score > bestScore)
        {
          bestScore = score;
          best = std::make_pair(s, d);
        }
      }
      if (prop)
      {
        std::string sigD = "(" + prop->signature() + ")";
        float score = Signature(sigS).isConvertibleTo(Signature(sigD));
        qiLogDebugF("scoring %s -> %s : %s", sigS, sigD, score);
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
    if (debugmode || best.second == PROP_MATCH)
      ptr.link = src->connect(srcSignals[best.first].uid(),
        qi::makeGenericFunction((boost::function<void(qi::GenericValue)>) boost::bind(&Behavior::transition, this, _1, t.uid)));
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

ObjectPtr Behavior::makeObject(const std::string& model, const std::string& factory)
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
    ObjectPtr s;
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
    if (method.empty())
      return s;

    qi::Future<ObjectPtr> o = s->call<ObjectPtr>(method);
    o.wait();
    return o; // will throw with backend error on failure
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

void Behavior::transition(qi::GenericValue arg, const std::string& transId)
{
  TransitionPtr t = _transitions[transId];
  if (t.debug)
    onTransition(transId, arg);

  qiLogDebug() << "Intercepted transition " << transId << " to " << t.targetMethod;
  if (!t.property.empty())
    t.target->setProperty(t.property, arg);
  else
  {
    qi::GenericFunctionParameters args;
    args.push_back(qi::GenericValuePtr(arg.type, arg.value));
    t.target->metaPost(t.targetMethod, args);
  }
}

void BehaviorModel::clear()
{
  nodes.clear();
  transitions.clear();
}
void BehaviorModel::load(std::istream& is)
{
  // uid a.b -> c.d
  // uid model factory
  char line[1024];
  while (is.good())
  {
    line[0] = 0;
    is.getline(line, 1024);
    std::string s(line);
    if (s.empty() || s[0] == '#')
      continue;
    std::stringstream st(s);
    std::string uid, p1, p2;
    bool t = false;
    st >> uid >> p1 >> p2;
    if (p2.empty())
      continue;
    if (p2 == "->")
    {
      t = true;
      st >> p2;
    }
    if (!t)
    {
      BehaviorModel::Node state;
      state.uid = uid;
      state.interface = p1;
      state.factory = p2;
      nodes[state.uid] = state;
    }
    else
    {
      BehaviorModel::Transition transition;
      transition.uid = uid;
      transition.src = splitString2(p1, '.');
      transition.dst = splitString2(p2, '.');
      transitions[transition.uid] = transition;
    }
  }
}
