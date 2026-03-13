#include "RPN.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Error: usage: ./RPN \"<expression>\"" << std::endl;
        return 1;
    }

    RPN rpn;
    rpn.evaluate(argv[1]);

    return 0;
}