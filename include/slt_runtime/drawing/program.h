#ifndef SLT_RUNTIME_RENDER_PROGRAM_H
#define SLT_RUNTIME_RENDER_PROGRAM_H

#include <functional>
#include <memory>

namespace slt {
namespace render {

struct Program {
  virtual ~Program() {}
};

using ProgramRef = std::shared_ptr<Program>;

struct ProgramUsage {
  ProgramUsage(ProgramRef const& prg);
  ~ProgramUsage();
};

}  // namespace render

void loadProgram(std::string const&, std::string const&,
                 std::function<void(render::ProgramRef)>);
}  // namespace slt
#endif