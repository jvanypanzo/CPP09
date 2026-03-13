#include "PmergeMe.hpp"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Error: usage: ./PmergeMe <n1> <n2> ... <nN>" << std::endl;
        return 1;
    }

    try
    {
        PmergeMe pm;
        pm.parseArgs(argc, argv);
        pm.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}