#include <QSystemTrayIcon>
#include <QApplication>
#include <QThread>
#include <qi/application.hpp>
#include <qi/anyobject.hpp>
#include <qi/anymodule.hpp>
#include <qi/session.hpp>


qiLogCategory("system.tray");

/* Invoke boost::function<void> in qt main thread
 *
 */
class Poster: public QObject
{
public:
  bool event(QEvent* e)
  {
    qiLogDebug() << "event " << e->type();
    if (e->type() != QEvent::User + 42)
      return false;
    std::vector<boost::function<void()> > queue2;
    {
      boost::mutex::scoped_lock lock(mutex);
      std::swap(queue, queue2);
    }
    qiLogDebug() << "processing messages: " << queue2.size();
    for (unsigned i=0; i<queue2.size(); ++i)
      queue2[i]();
    return true;
  }

  static QObject* poster()
  {
    static Poster* inst = 0;
    if (!inst)
    {
      inst = new Poster();
      inst->moveToThread(QApplication::instance()->thread());
    }
    return inst;
  };

  static void post(boost::function<void()> f)
  {
    qiLogDebug() << "post";
    {
      boost::mutex::scoped_lock lock(mutex);
      queue.push_back(f);
    }
    QApplication::postEvent(poster(), new QEvent((QEvent::Type)(QEvent::User + 42)));
  }
  static boost::mutex mutex;
  static std::vector<boost::function<void()> > queue;
};

boost::mutex Poster::mutex;
std::vector<boost::function<void()> > Poster::queue;

class Tray
{
public:
  Tray();
  void setIcon(const std::string& path);
  void setToolTip(const std::string& path);
  void showMessage(const std::string& title, const std::string& message, int icon, int msTimeout);
  qi::Property<bool> visible;
  void setVisible(bool);
private:
  QSystemTrayIcon _tray;
};

QI_REGISTER_OBJECT(Tray, setIcon, setToolTip, showMessage, visible, setVisible);
QI_REGISTER_PACKAGE_OBJECT_FACTORY_BUILDER("Tray", Tray);

Tray::Tray()
{
  visible.connect(boost::bind(&Tray::setVisible, this, _1));
}

void Tray::setVisible(bool v)
{
  if (QApplication::instance()->thread()  != QThread::currentThread())
    return Poster::post(boost::bind(&Tray::setVisible, this, v));
  _tray.setVisible(v);
}

void Tray::setIcon(const std::string& path)
{
  if (QApplication::instance()->thread()  != QThread::currentThread())
    return Poster::post(boost::bind(&Tray::setIcon, this, path));
  QIcon icon(QString(path.c_str()));
  _tray.setIcon(icon);
}

void Tray::setToolTip(const std::string& path)
{
  _tray.setToolTip(QString(path.c_str()));
}

void Tray::showMessage(const std::string& title, const std::string& message, int icon, int msTimeout)
{
  if (QApplication::instance()->thread()  != QThread::currentThread())
    return Poster::post(boost::bind(&Tray::showMessage, this, title, message, icon, msTimeout));
  _tray.showMessage(QString(title.c_str()), QString(message.c_str()), (QSystemTrayIcon::MessageIcon) icon, msTimeout);
}


int main(int argc, char** argv)
{
  qi::Application app(argc, argv);
  QApplication a(argc, argv);
  std::string name = "TrayService";
  if (argc > 2)
    name = argv[2];
  qi::Session s;
  s.listen("tcp://0.0.0.0:0");
  s.connect(argv[1]);

  s.registerService(name, qi::import("Tray")->createObject("TrayService"));


  //s.registerService(name, qi::GenericValuePtr(new Tray()).toObject());
  a.exec();
}
