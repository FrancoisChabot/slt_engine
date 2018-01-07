#include "slt_runtime/gui/widgets/image.h"
#include "slt_runtime/resources.h"

namespace slt {
  namespace gui {

    Image::Image(std::string const& package, std::string const& asset) {
      getRegistry(package)->get<render::Texture>(asset, [self = get_self_nulling_ptr()](auto t){
        if (self) {
          static_cast<Image*>(self.get())->texture_ = std::move(t);
        }
      });
    }

    void Image::render(WidgetRenderer& r, WidgetRenderRequest const& rr) {
      if(texture_) {
        std::vector<Vertex> vertices;
        std::vector<Index> indices = { 0, 1, 2, 2, 1, 3 };

        vertices.reserve(4);

        Vertex v;
        v.color = slt::white;

        v.pos = rr.pos;
        v.uv = { 0,1 };
        vertices.emplace_back(v);

        v.pos.x = rr.pos.x + rr.size.x;
        v.uv = { 1,1 };
        vertices.emplace_back(v);

        v.pos.x = rr.pos.x ;
        v.pos.y = rr.pos.y + rr.size.y;
        v.uv = { 0,0 };
        vertices.emplace_back(v);

        v.pos = rr.pos + rr.size;
        v.uv = { 1,0 };
        vertices.emplace_back(v);

        r.draw(vertices, indices, texture_, nullptr);
      }
    }
  }
}
