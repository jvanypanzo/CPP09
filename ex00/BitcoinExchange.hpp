
#ifndef BITCOINEXCHANGE_HPP
# define BITCOINEXCHANGE_HPP

# include <iostream>
# include <fstream>
# include <sstream>
# include <string>
# include <map>

// std::map foi escolhido porque:
// 1. Armazena pares chave-valor (data -> preco)
// 2. Mantém as chaves ORDENADAS automaticamente (árvore binária de busca)
// 3. Tem o método lower_bound() que encontra a chave mais próxima <= ao valor
//    procurado — essencial para a regra "data inferior mais próxima"

class BitcoinExchange
{
    public:
        BitcoinExchange();
        BitcoinExchange(const BitcoinExchange &other);
        BitcoinExchange &operator=(const BitcoinExchange &other);
        ~BitcoinExchange();

        void    loadDatabase(const std::string &csvFile);
        void    processInput(const std::string &inputFile);

    private:
        // Mapa: string da data ("YYYY-MM-DD") -> preço do bitcoin (float)
        std::map<std::string, float> _db;

        bool    isValidDate(const std::string &date) const;
        bool    isValidValue(const std::string &valueStr, float &outValue) const;
        float   getPriceOnDate(const std::string &date) const;
};

#endif