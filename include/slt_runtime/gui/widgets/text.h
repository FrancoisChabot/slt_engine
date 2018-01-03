#ifndef MYGUI_WIDGETS_TEXT_H
#define MYGUI_WIDGETS_TEXT_H

#include "slt_runtime/gui/widget.h"
#include "slt_runtime/gui/font.h"
#include "slt_runtime/gui/data_binding.h"

#include <string>

namespace slt {
  namespace gui {
    enum class TextHAlign {
      Left,
      Center,
      Right,
    };

    enum class TextVAlign {
      Top,
      Middle,
      MiddleBaseline,
      Bottom,
      BottomBaseline,
    };

    class Text : public Widget {
    public:
      static FontRef default_font;

      template<typename T>
      Text(T&& data, FontRef font = FontRef())
        : value_(std::forward<T>(data))
        , font_(font) {
        if (!font_) {
          font_ = default_font;
        }
      }

      void render(WidgetRenderer&, WidgetRenderRequest const& r) override;

      Vec2 getMinSize() override {
        return getBoxSize();
      }

      template<typename T>
      void setColor(T&& c) {
        color_ = BindableValue<Color>(std::forward<T>(c));
      }

    private:

      Vec2 getBoxSize() {
        Vec2 result = { 0, 0 };
        auto current_val = value_.get();

        for (auto c : current_val) {
          auto advance = font_->getGlyphAdvance(c);
          result.x += advance;
        }
        return result;
      }

      TextHAlign h_align = TextHAlign::Center;
      TextVAlign v_align = TextVAlign::Middle;
      BindableValue<std::string> value_;
      BindableValue<Color> color_ = black;

      FontRef font_;
    };
  }
}

#endif
