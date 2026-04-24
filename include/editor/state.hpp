#pragma once
#include "types.hpp"
#include <stack>
#include <vector>

namespace editor {
    extern std::stack<Action> action_history;
    extern std::vector<SearchMatch> found_occurrences;
    extern int current_occurrence_index;
}