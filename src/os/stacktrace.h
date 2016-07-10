#ifndef REAPER_HW_STACKTRACE_H
#define REAPER_HW_STACKTRACE_H

#include <vector>
#include <string>

namespace reaper {
namespace os {

std::vector<std::string> get_stack_frames();

void load_symbols_prog(std::string prog);
void load_symbols_file(std::string file);

void catch_segfault();

}
}

#endif

