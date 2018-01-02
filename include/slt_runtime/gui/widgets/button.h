#ifndef MYGUI_WIDGETS_BUTTON_H
#define MYGUI_WIDGETS_BUTTON_H

#include "slt/gui/config.h"
#include "slt/gui/widget.h"

namespace slt {

  template<typename RENDERER_T>
  class Button : public Widget<RENDERER_T> {
  public:
  private:
    std::function<void(void)> action_cb_;
  };
}

#endif