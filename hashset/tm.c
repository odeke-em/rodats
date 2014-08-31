#include "errors.h"

int main() {
    raiseError("Failing here: %d\n", 10);
    return 0;
}
