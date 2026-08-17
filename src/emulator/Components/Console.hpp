#include "core/ProcessorState.hpp"

#include <ftxui/ftxui.hpp>

namespace emulator {
  class Console : public ftxui::ComponentBase {
  public:
    Console(core::ProcessorState& state);

    ftxui::Element OnRender() override;
    bool OnEvent(ftxui::Event) override;

  private:
    core::ProcessorState& m_state;
  };
}
