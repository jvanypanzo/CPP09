#include "BitcoinExchange.hpp"

// ─── Forma Canônica Ortodoxa ─────────────────────────────────────────────────
// O módulo exige que toda classe implemente os 4 membros canônicos:
// construtor default, construtor de cópia, operador de atribuição e destrutor.

BitcoinExchange::BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(const BitcoinExchange &other)
    : _db(other._db) {}

BitcoinExchange &BitcoinExchange::operator=(const BitcoinExchange &other)
{
    if (this != &other)
        _db = other._db;
    return *this;
}

BitcoinExchange::~BitcoinExchange() {}

// ─── loadDatabase ────────────────────────────────────────────────────────────
// Lê o CSV fornecido com o subject. Formato esperado:
//   date,exchange_rate
//   2009-01-02,0
//   2009-01-09,0
//   ...
// Cada linha é parseada e inserida no std::map<string, float> _db.
// Como o map ordena por string e as datas estão no formato ISO (YYYY-MM-DD),
// a ordenação lexicográfica equivale à ordenação cronológica — perfeito!

void BitcoinExchange::loadDatabase(const std::string &csvFile)
{
    std::ifstream file(csvFile.c_str());
    if (!file.is_open())
        throw std::runtime_error("Error: could not open database file.");

    std::string line;
    std::getline(file, line); // pula o cabeçalho "date,exchange_rate"

    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        // Encontra a vírgula separadora
        std::size_t sep = line.find(',');
        if (sep == std::string::npos)
            continue;

        std::string date  = line.substr(0, sep);
        std::string price = line.substr(sep + 1);

        // Converte o preço para float usando stringstream (C++98 sem stof)
        float value;
        std::istringstream iss(price);
        if (!(iss >> value))
            continue;

        _db[date] = value;
    }

    if (_db.empty())
        throw std::runtime_error("Error: database is empty or invalid.");
}

// ─── isValidDate ─────────────────────────────────────────────────────────────
// Valida o formato YYYY-MM-DD e os intervalos de mês (1-12) e dia (1-31).
// Verificação básica — não checa meses com 30 dias nem anos bissextos,
// pois o subject não exige precisão total de calendário.

bool BitcoinExchange::isValidDate(const std::string &date) const
{
    if (date.size() != 10)
        return false;
    if (date[4] != '-' || date[7] != '-')
        return false;

    // Verifica que os 8 dígitos são todos numéricos
    for (int i = 0; i < 10; i++)
    {
        if (i == 4 || i == 7) continue;
        if (!std::isdigit(date[i]))
            return false;
    }

    // Extrai e valida mês e dia
    std::istringstream issM(date.substr(5, 2));
    std::istringstream issD(date.substr(8, 2));
    int month, day;
    issM >> month;
    issD >> day;

    if (month < 1 || month > 12)
        return false;
    if (day < 1 || day > 31)
        return false;
    return true;
}

// ─── isValidValue ─────────────────────────────────────────────────────────────
// Regras do subject:
//   - deve ser float ou inteiro positivo
//   - entre 0 e 1000 (inclusive)
// Usa stringstream para tentar converter. Verifica também se há caracteres
// inválidos após o número (ex: "1.2abc" seria rejeitado pela flag fail/eof).

bool BitcoinExchange::isValidValue(const std::string &valueStr, float &outValue) const
{
    std::istringstream iss(valueStr);
    iss >> outValue;

    if (iss.fail())
        return false;

    // Certifica que não sobrou lixo após o número
    std::string leftover;
    iss >> leftover;
    if (!leftover.empty())
        return false;

    if (outValue < 0)
    {
        std::cerr << "Error: not a positive number." << std::endl;
        return false;
    }
    if (outValue > 1000)
    {
        std::cerr << "Error: too large a number." << std::endl;
        return false;
    }
    return true;
}

// ─── getPriceOnDate ──────────────────────────────────────────────────────────
// PONTO CENTRAL DO EXERCÍCIO: encontrar a data inferior mais próxima.
//
// std::map::lower_bound(key) retorna um iterador para o primeiro elemento
// com chave >= key. Então:
//
//   - Se it == begin() e *it != key → não existe data anterior → erro
//   - Se it aponta exatamente para key → correspondência exata → usa esse preço
//   - Caso contrário → recua um passo (--it) para pegar a data anterior
//
// Exemplo visual do mapa ordenado:
//   2011-01-01 -> 0.30
//   2011-01-03 -> 0.30   <─ lower_bound("2011-01-03") aponta aqui (exato)
//   2011-06-15 -> 15.40
//
// Para data "2011-01-05" (não existe):
//   lower_bound retorna o iterador de "2011-06-15"
//   --it aponta para "2011-01-03" → usa 0.30

float BitcoinExchange::getPriceOnDate(const std::string &date) const
{
    std::map<std::string, float>::const_iterator it = _db.lower_bound(date);

    // Caso 1: correspondência exata
    if (it != _db.end() && it->first == date)
        return it->second;

    // Caso 2: data anterior ao início do banco
    if (it == _db.begin())
    {
        std::cerr << "Error: date out of range => " << date << std::endl;
        return -1;
    }

    // Caso 3: recua para a data imediatamente anterior
    --it;
    return it->second;
}

// ─── processInput ────────────────────────────────────────────────────────────
// Lê o arquivo de entrada linha a linha.
// Formato esperado: "YYYY-MM-DD | valor"
// Cada linha é validada e o resultado é impresso ou um erro é exibido.

void BitcoinExchange::processInput(const std::string &inputFile)
{
    std::ifstream file(inputFile.c_str());
    if (!file.is_open())
    {
        std::cerr << "Error: could not open file." << std::endl;
        return;
    }

    std::string line;
    std::getline(file, line); // pula cabeçalho "date | value"

    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        // Procura o separador " | "
        std::size_t sep = line.find(" | ");
        if (sep == std::string::npos)
        {
            // Se não achou " | ", pode ser formato errado ou data sem valor
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }

        std::string date     = line.substr(0, sep);
        std::string valueStr = line.substr(sep + 3);

        // Valida a data
        if (!isValidDate(date))
        {
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }

        // Valida o valor (erros de range já são impressos dentro da função)
        float value;
        if (!isValidValue(valueStr, value))
            continue;

        // Busca o preço no banco de dados
        float price = getPriceOnDate(date);
        if (price < 0)
            continue; // erro já impresso em getPriceOnDate

        // Imprime resultado: "2011-01-03 => 3 = 0.9"
        std::cout << date << " => " << value << " = " << value * price << std::endl;
    }
}