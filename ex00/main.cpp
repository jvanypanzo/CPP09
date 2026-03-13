#include "BitcoinExchange.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Error: could not open file." << std::endl;
        return 1;
    }

    try
    {
        BitcoinExchange btc;

        // Carrega o banco de dados CSV (fornecido com o subject)
        btc.loadDatabase("data.csv");

        // Processa o arquivo de entrada passado como argumento
        btc.processInput(argv[1]);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}