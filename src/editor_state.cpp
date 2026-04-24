#include "../include/editor/state.hpp"

namespace editor {
    std::stack<Action> action_history;
    std::vector<SearchMatch> found_occurrences;
    int current_occurrence_index = 0;
}