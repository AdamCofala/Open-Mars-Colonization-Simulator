#include "Pipe.h"

Pipe::Pipe(int tx, int ty) {
    // Domyœlna rura zajmuje 1x1 kafelka, tekstura "pipe"
    init(tx, ty, "pipe", 1, 1);
}