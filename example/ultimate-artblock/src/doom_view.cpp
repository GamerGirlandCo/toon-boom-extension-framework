#include "./include/doom_view.hpp"
#include "common.h"

DoomView::DoomView() : TUWidgetLayoutViewBase<ToonDoomWidget>() {}

DoomView::~DoomView() {}

QString DoomView::displayName() const { return "Doom!?"; }

ToonDoomWidget *DoomView::createWidget() { return new ToonDoomWidget(); }

app_proc_t real_app_proc = app_proc;

namespace toon_doom {
int app_proc(app_t *app, void *user_data) {
  APP_U32 canvas[320 * 200];
  memset(canvas, 0xc0, sizeof(canvas));
  auto asDoomThread = reinterpret_cast<DoomThread *>(user_data);
  asDoomThread->setApp(app);
  // return 0;
  auto result = real_app_proc(app, user_data); 
  thread_atomic_int_store(&app_running, 0);
  std::cout << "exiting app_proc" << std::endl;
  return result;
}
int app_proc_thread(void *user_data) {
  auto asDoomThread = reinterpret_cast<DoomThread *>(user_data);
  asDoomThread->setId(GetCurrentThreadId());
	auto result = app_run(app_proc, user_data, nullptr, nullptr, nullptr);
  return result;
}
} // namespace toon_doom