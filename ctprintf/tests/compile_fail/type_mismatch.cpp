#include "../../include/ctprintf/format.hpp"

struct Output {
    void put(char) const
    {
        // This fixture only satisfies ctprintf::Output; it is never executed.
    }
};

int main()
{
    Output output;
    ctprintf::format(output, "%08x", "not an integer");
}
