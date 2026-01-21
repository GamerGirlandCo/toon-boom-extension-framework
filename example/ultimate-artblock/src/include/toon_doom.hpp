#pragma once
#include <QtCore/QPointer>
#include <QtCore/qnamespace.h>

#include <QtCore/QEvent>
#include <QtCore/QThread>
#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>
#include <set>
#include <toon_boom/ext/layout.hpp>
#include <toon_boom/ext/util.hpp>

#include "./common.h"
#include "./util.hpp"

typedef int (*app_proc_t)(app_t *, void *);

namespace toon_doom {
int app_proc(app_t *app, void *user_data);
int app_proc_thread(void *user_data);
}; // namespace toon_doom
class ToonDoomWidget;

class DoomThread : public QThread {
  Q_OBJECT
public:
  DoomThread(ToonDoomWidget *widget, QObject *parent = nullptr)
      : QThread(parent) {
    m_widget = widget;
  }
  ~DoomThread() {
    M_QuitResponse('y');
    thread_atomic_int_store(&app_running, 0);
    maketic = 0;
    gametic = 0;
    if (isRunning()) {
      terminate();
    }
  }
  void startGame(app_t *app) { emit gameStarted(m_widget, app); }
  void setApp(app_t *app) { this->app = app; }
  DWORD getId() { return tid; }
  void setId(DWORD tid) { this->tid = tid; }

private:
signals:
  void gameStarted(ToonDoomWidget *widget, app_t *app);
  void gameExited(const int &result);

protected:
  QPointer<ToonDoomWidget> m_widget;
  void run() override {
    tid = GetCurrentThreadId();
    myargc = 0;
    myargv = nullptr;
    thread_signal_init(&vblank_signal);
    thread_mutex_init(&mus_mutex);
    thread_atomic_int_store(&app_running, 1);
    threadPtr = thread_create(&toon_doom::app_proc_thread, this,
                              THREAD_STACK_SIZE_DEFAULT);
    int result =
        thread_signal_wait(&vblank_signal, THREAD_SIGNAL_WAIT_INFINITE);
    startGame(app);
    D_DoomMain();
    emit gameExited(result);
  }
  thread_ptr_t threadPtr = nullptr;
  DWORD tid = 0;
  app_t *app = nullptr;
};

class ToonDoomWidget : public QWidget {
  Q_OBJECT
public:
  ToonDoomWidget(QWidget *parent = nullptr) : QWidget(parent) {
    setPalette(QPalette(QColor(0, 0, 0)));
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setAutoFillBackground(true);
    setMinimumSize(320, 200);
    setWindowTitle("Doom!?");
    m_layout = new QVBoxLayout(this);
    m_layout->setAlignment(Qt::AlignCenter);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    setFocusPolicy(Qt::StrongFocus);
  }
  ~ToonDoomWidget() {
    QObject::disconnect(static_cast<QApplication *>(QApplication::instance()),
                        &QApplication::focusChanged, this,
                        &ToonDoomWidget::focusChanged);
    auto inp =
        AttachThreadInput(GetCurrentThreadId(), doomThread->getId(), false);
    util::debug::out << "~ToonDoomWidget AttachThreadInput: " << inp
                     << std::endl;
    delete doomThread;
  }
  void setApp(app_t *app) {
    this->app = app;
    init();
  }
  app_t *getApp() const { return app; }

public slots:
  void start() {
    if (doomThread != nullptr && doomThread->isRunning()) {
      return;
    }
    doomThread = new DoomThread(this, this);
    connect(doomThread, &DoomThread::gameExited, this,
            &ToonDoomWidget::gameExited);
    connect(doomThread, &DoomThread::gameStarted, this,
            &ToonDoomWidget::gameStarted);
    doomThread->start();
  }
  void gameStarted(ToonDoomWidget *widget, app_t *app) { widget->setApp(app); }
  void gameExited(const int &result) {}

private:
  QWidget *doomWidget = nullptr;
  app_t *app = nullptr;
  QWindow *m_window = nullptr;
  DoomThread *doomThread = nullptr;
  QVBoxLayout *m_layout = nullptr;

protected:
  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);
    updateGeometry();
  }
  void moveEvent(QMoveEvent *event) override {
    QWidget::moveEvent(event);
    updateGeometry();
  }
  void focusInEvent(QFocusEvent *event) override {
    util::debug::out << "focusInEvent: " << event->type() << std::endl;
    if (app) {
      app->has_focus = true;
      SetFocus(app->hwnd);
    }
    QWidget::focusInEvent(event);
  }

  bool eventFilter(QObject *obj, QEvent *event) override {
    QMetaEnum metaEnum = QMetaEnum::fromType<QEvent::Type>();
    util::debug::out << "eventFilter: " << metaEnum.valueToKey(event->type())
                     << " obj: " << obj->metaObject()->className() << std::endl;
    if (event->type() == QEvent::WindowActivate ||
        (event->type() == QEvent::FocusIn && obj != this)) {
      this->setFocus(Qt::OtherFocusReason);
    }
    if (obj == this && (event->type() == QEvent::FocusOut ||
                        event->type() == QEvent::FocusAboutToChange)) {
      event->ignore();
      setFocus();
      return true;
    }
    return QWidget::eventFilter(obj, event);
  }
  bool event(QEvent *event) override {
    if (event->type() == QEvent::FocusOut ||
        event->type() == QEvent::FocusAboutToChange) {
      util::debug::out << "focus out" << std::endl;
      setEnabled(true);
      if (event->type() == QEvent::FocusOut) {
        auto asFocusEvent = static_cast<QFocusEvent *>(event);
        util::debug::out << "focus out reason: " << asFocusEvent->reason()
                         << std::endl;
        if (asFocusEvent->reason() == Qt::OtherFocusReason) {
          event->ignore();
          setFocus();
          QApplication::processEvents();
        }
      } else if (event->type() == QEvent::FocusAboutToChange) {
        event->ignore();
      }
      return true;
    }
    return QWidget::event(event);
  }

private:
  std::pair<QKeyEvent *, QKeyEvent *> createEscapeKeyEvents() {
    return std::pair<QKeyEvent *, QKeyEvent *>(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier),
        new QKeyEvent(QEvent::KeyRelease, Qt::Key_Escape, Qt::NoModifier));
  }

  bool isMouseMoveEvent(QEvent *event) {
    return event->type() == QEvent::MouseMove ||
           event->type() == QEvent::Enter || event->type() == QEvent::Leave ||
           event->type() == QEvent::TabletMove ||
           event->type() == QEvent::NonClientAreaMouseMove ||
           event->type() == QEvent::GrabMouse ||
           event->type() == QEvent::HoverMove ||
           event->type() == QEvent::HoverEnter ||
           event->type() == QEvent::HoverLeave ||
           event->type() == QEvent::GraphicsSceneHoverMove ||
           event->type() == QEvent::GraphicsSceneHoverEnter ||
           event->type() == QEvent::GraphicsSceneHoverLeave ||
           event->type() == QEvent::GraphicsSceneMouseMove ||
           event->type() == QEvent::GraphicsSceneLeave;
  }
  std::set<QWidget *> m_ancestors;
private slots:
  void updateGeometry() {
    if (m_window != nullptr) {
      doomWidget->setGeometry(this->rect());
      m_window->setGeometry(doomWidget->rect());
    }
  }
  void windowClosed() {
    util::debug::out << "window closed" << std::endl;
    delete doomThread;
    doomThread = nullptr;
  }
  void init() {
    m_window = QWindow::fromWinId(WId(app->hwnd));
    Qt::WindowFlags nflags = Qt::WindowType::Window | Qt::WindowType::Widget |
                             Qt::WindowType::FramelessWindowHint |
                             Qt::CustomizeWindowHint;
    const auto dpr = m_window->devicePixelRatio();
    // m_window->setFlags(m_window->flags() | nflags);
    m_window->setGeometry(this->rect());
    connect(m_window, &QWindow::close, this, &ToonDoomWidget::windowClosed);
    doomWidget =
        QWidget::createWindowContainer(m_window, this, Qt::WindowType::Widget);
    doomWidget->setSizePolicy(QSizePolicy::MinimumExpanding,
                              QSizePolicy::MinimumExpanding);
    util::debug::out << "doomwidget == window: "
                     << (doomWidget->windowHandle() == m_window) << std::endl;
    auto flags = doomWidget->windowFlags();

    doomWidget->hide();
    m_layout->addWidget(doomWidget, 1);
    util::debug::out << "doom thread id: " << doomThread->getId() << std::endl;
    auto inp =
        AttachThreadInput(GetCurrentThreadId(), doomThread->getId(), true);
    util::debug::out << "AttachThreadInput: " << inp << std::endl;
    doomWidget->show();
    auto parentWindow = window();
    updateGeometry();

    {
      // add widget hierarchy to a set for focus tracking
      QWidget *p = doomWidget;
      while (p != nullptr) {
        util::debug::out << "widget @ " << util::debug::addrToHex(p) << ": "
                         << p->metaObject()->className() << " parent: "
                         << util::debug::addrToHex(p->parentWidget())
                         << std::endl;
        m_ancestors.insert(p);

        p = p->parentWidget();
      }
    }
    installEventFilter(this);
    doomWidget->setFocusPolicy(Qt::StrongFocus);
    QObject::connect(static_cast<QApplication *>(QApplication::instance()),
                     &QApplication::focusChanged, this,
                     &ToonDoomWidget::focusChanged);
  }

  void focusChanged(QWidget *old, QWidget *now) {
    if (m_ancestors.contains(old) && !m_ancestors.contains(now)) {
      app->has_focus = false;
      M_StartControlPanel();
    }
  }
};