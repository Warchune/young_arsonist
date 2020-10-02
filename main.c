#include "young_arsonist.h"

int main() {
    load();
    build_graph();
    find_shortest_paths();
    solve();
    save();
    return 0;
}