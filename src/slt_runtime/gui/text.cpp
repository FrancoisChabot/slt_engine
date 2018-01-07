#include "slt_runtime/gui/widgets/text.h"

namespace slt {
  namespace gui {
  FontRef Text::default_font;

  void Text::render(WidgetRenderer& r, WidgetRenderRequest const& rr) {

      std::vector<Vertex> vertices;
      std::vector<Index> indices;

      Vec2 cursor{ 0,0 };
      auto box_size = getBoxSize();
      switch(h_align) {
        case TextHAlign::Center:
          cursor.x = (rr.size.x - box_size.x) / 2.0f;
        break;
        case TextHAlign::Right:
          cursor.x = (rr.size.y - box_size.y);
        break;

        case TextHAlign::Left:
        default:
          break;
      }


      cursor += rr.pos;
      switch (v_align) {
      case TextVAlign::Top:
        cursor.y = rr.size.y - font_->getAscent();
        break;
      case TextVAlign::Middle:
      {
        float total_height = font_->getAscent() + font_->getDescent();
        cursor.y = rr.size.y / 2.0f - (total_height * 0.5f - font_->getDescent());
      }
        break;
      case TextVAlign::MiddleBaseline:
        cursor.y = rr.size.y / 2.0f;
        break;

      case TextVAlign::Bottom:
        cursor.y = font_->getDescent();
        break;
      case TextVAlign::BottomBaseline:
      default:
        break;
      }

      Vertex v;
      v.color = color_.get();
      
      for(auto c : value_.get()) {
        auto glyph = font_->getGlyph(c);
        
        Index ref_id = (Index)vertices.size();

        v.pos.x = cursor.x + glyph.top_left_offset.x;
        v.pos.y = cursor.y + glyph.top_left_offset.y;
        v.uv.x = glyph.top_left_uv.x;
        v.uv.y = glyph.top_left_uv.y;
        vertices.emplace_back(v);

        v.pos.x = cursor.x + glyph.bottom_right_offset.x;
        v.pos.y = cursor.y + glyph.top_left_offset.y;
        v.uv.x = glyph.top_right_uv.x;
        v.uv.y = glyph.top_right_uv.y;
        vertices.emplace_back(v);

        v.pos.x = cursor.x + glyph.top_left_offset.x;
        v.pos.y = cursor.y + glyph.bottom_right_offset.y;
        v.uv.x = glyph.bottom_left_uv.x;
        v.uv.y = glyph.bottom_left_uv.y;
        vertices.emplace_back(v);

        v.pos.x = cursor.x + glyph.bottom_right_offset.x;
        v.pos.y = cursor.y + glyph.bottom_right_offset.y;
        v.uv.x = glyph.bottom_right_uv.x;
        v.uv.y = glyph.bottom_right_uv.y;
        vertices.emplace_back(v);

        indices.emplace_back(ref_id+0);
        indices.emplace_back(ref_id+1);
        indices.emplace_back(ref_id+2);
        indices.emplace_back(ref_id+2);
        indices.emplace_back(ref_id+1);
        indices.emplace_back(ref_id+3);

        cursor += glyph.advance;
      }

       r.draw(std::move(vertices), std::move(indices), nullptr, font_->getTexture());
    }
  }
  }
