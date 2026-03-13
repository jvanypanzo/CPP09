#include "PmergeMe.hpp"
#include <algorithm>
#include <stdexcept>
#include <sys/time.h>

// ─── Forma Canônica Ortodoxa ─────────────────────────────────────────────────

PmergeMe::PmergeMe() {}

PmergeMe::PmergeMe(const PmergeMe &other)
    : _vec(other._vec), _deq(other._deq) {}

PmergeMe &PmergeMe::operator=(const PmergeMe &other)
{
    if (this != &other)
    {
        _vec = other._vec;
        _deq = other._deq;
    }
    return *this;
}

PmergeMe::~PmergeMe() {}

// ─── parseArgs ───────────────────────────────────────────────────────────────
// Converte os argumentos argv em inteiros positivos.
// Rejeita: negativos, não-numéricos, overflow.

void PmergeMe::parseArgs(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        std::string token(argv[i]);

        // Rejeita strings vazias ou com caractere negativo
        if (token.empty() || token[0] == '-')
            throw std::runtime_error("Error: invalid argument: " + token);

        // Verifica que todos os caracteres são dígitos
        for (size_t j = 0; j < token.size(); j++)
        {
            if (!std::isdigit(token[j]))
                throw std::runtime_error("Error: invalid argument: " + token);
        }

        std::istringstream iss(token);
        long num;
        iss >> num;

        if (num <= 0 || num > 2147483647)
            throw std::runtime_error("Error: out of range: " + token);

        _vec.push_back(static_cast<int>(num));
        _deq.push_back(static_cast<int>(num));
    }

    if (_vec.empty())
        throw std::runtime_error("Error: no input provided.");
}

// ─── jacobsthalSequence ──────────────────────────────────────────────────────
// Gera os índices de inserção baseados na sequência de Jacobsthal até n.
//
// A sequência de Jacobsthal: J(0)=0, J(1)=1, J(n) = J(n-1) + 2*J(n-2)
//   → 0, 1, 1, 3, 5, 11, 21, 43, 85, 171, 341, ...
//
// Usamos ela para determinar a ORDEM em que os elementos "pendentes"
// são inseridos na cadeia principal. Inserir na ordem de Jacobsthal
// garante que cada busca binária subsequente não excede o número de
// comparações da anterior — minimizando o total.
//
// Exemplo com 5 pendentes (índices 0..4):
//   Jacobsthal até 5: 0,1,3  (valores <= 5)
//   Ordem de inserção: pend[3], pend[2], pend[1], pend[0] → pend[4]
//   (insere do maior grupo para o menor, preenchendo o intervalo)

std::vector<int> PmergeMe::jacobsthalSequence(int n) const
{
    std::vector<int> jac;
    jac.push_back(0);
    jac.push_back(1);

    while (true)
    {
        int next = jac[jac.size() - 1] + 2 * jac[jac.size() - 2];
        if (next >= n)
            break;
        jac.push_back(next);
    }
    return jac;
}

// ─── Ford-Johnson para std::vector ──────────────────────────────────────────

// insertionVector: busca binária + inserção na posição correta
// std::lower_bound encontra a primeira posição onde o valor pode ser
// inserido mantendo a ordem — O(log n) comparações, O(n) deslocamentos.
void PmergeMe::insertionVector(std::vector<int> &main, std::vector<int> &pend)
{
    if (pend.empty())
        return;

    // Primeiro pendente (pend[0]) sempre é inserido no início
    // pois seu par (main[0]) já está na cadeia, e pend[0] <= main[0]
    std::vector<int>::iterator pos = std::lower_bound(main.begin(), main.end(), pend[0]);
    main.insert(pos, pend[0]);

    if (pend.size() == 1)
        return;

    // Gera a sequência de Jacobsthal para determinar a ordem de inserção
    std::vector<int> jac = jacobsthalSequence(static_cast<int>(pend.size()));

    std::vector<bool> inserted(pend.size(), false);
    inserted[0] = true;

    // Processa em grupos de Jacobsthal
    for (size_t k = 1; k < jac.size(); k++)
    {
        int idx = jac[k];
        if (idx >= static_cast<int>(pend.size()))
            idx = static_cast<int>(pend.size()) - 1;

        // Insere do índice Jacobsthal até o último não inserido do grupo anterior
        for (int j = idx; j >= jac[k - 1]; j--)
        {
            if (j < static_cast<int>(pend.size()) && !inserted[j])
            {
                pos = std::lower_bound(main.begin(), main.end(), pend[j]);
                main.insert(pos, pend[j]);
                inserted[j] = true;
            }
        }
    }

    // Insere quaisquer pendentes que sobraram além do último Jacobsthal
    for (size_t j = 0; j < pend.size(); j++)
    {
        if (!inserted[j])
        {
            pos = std::lower_bound(main.begin(), main.end(), pend[j]);
            main.insert(pos, pend[j]);
        }
    }
}

// sortVector: implementação recursiva do Ford-Johnson
//
// Passo 1: Forma pares (main[i], pend[i]) onde main[i] >= pend[i]
// Passo 2: Ordena recursivamente a cadeia main
// Passo 3: Insere os pendentes com busca binária por ordem de Jacobsthal

void PmergeMe::sortVector(std::vector<int> &v)
{
    size_t n = v.size();

    // Base: 0 ou 1 elemento já está ordenado
    if (n <= 1)
        return;

    // Caso especial: 2 elementos — troca direta
    if (n == 2)
    {
        if (v[0] > v[1])
            std::swap(v[0], v[1]);
        return;
    }

    bool hasStraggler = (n % 2 != 0);
    int  straggler    = hasStraggler ? v[n - 1] : 0;
    if (hasStraggler)
        v.pop_back();

    // Passo 1: forma pares, coloca o maior em main, menor em pend
    std::vector<int> main_chain;
    std::vector<int> pend_chain;

    for (size_t i = 0; i < v.size(); i += 2)
    {
        if (v[i] > v[i + 1])
        {
            main_chain.push_back(v[i]);
            pend_chain.push_back(v[i + 1]);
        }
        else
        {
            main_chain.push_back(v[i + 1]);
            pend_chain.push_back(v[i]);
        }
    }

    // Passo 2: ordena recursivamente a cadeia dos maiores
    sortVector(main_chain);

    // Passo 3: insere os menores na cadeia ordenada
    insertionVector(main_chain, pend_chain);

    // Se tinha elemento ímpar sobrando, insere também
    if (hasStraggler)
    {
        std::vector<int>::iterator pos =
            std::lower_bound(main_chain.begin(), main_chain.end(), straggler);
        main_chain.insert(pos, straggler);
    }

    v = main_chain;
}

// ─── Ford-Johnson para std::deque ────────────────────────────────────────────
// Lógica idêntica ao vector, mas usando std::deque.
// std::deque::insert no meio é O(n) como vector, mas com overhead maior
// devido à sua estrutura em blocos — isso aparece no tempo de execução.

void PmergeMe::insertionDeque(std::deque<int> &main, std::deque<int> &pend)
{
    if (pend.empty())
        return;

    std::deque<int>::iterator pos =
        std::lower_bound(main.begin(), main.end(), pend[0]);
    main.insert(pos, pend[0]);

    if (pend.size() == 1)
        return;

    std::vector<int> jac = jacobsthalSequence(static_cast<int>(pend.size()));

    std::vector<bool> inserted(pend.size(), false);
    inserted[0] = true;

    for (size_t k = 1; k < jac.size(); k++)
    {
        int idx = jac[k];
        if (idx >= static_cast<int>(pend.size()))
            idx = static_cast<int>(pend.size()) - 1;

        for (int j = idx; j >= jac[k - 1]; j--)
        {
            if (j < static_cast<int>(pend.size()) && !inserted[j])
            {
                pos = std::lower_bound(main.begin(), main.end(), pend[j]);
                main.insert(pos, pend[j]);
                inserted[j] = true;
            }
        }
    }

    for (size_t j = 0; j < pend.size(); j++)
    {
        if (!inserted[j])
        {
            pos = std::lower_bound(main.begin(), main.end(), pend[j]);
            main.insert(pos, pend[j]);
        }
    }
}

void PmergeMe::sortDeque(std::deque<int> &d)
{
    size_t n = d.size();

    if (n <= 1)
        return;

    if (n == 2)
    {
        if (d[0] > d[1])
            std::swap(d[0], d[1]);
        return;
    }

    bool hasStraggler = (n % 2 != 0);
    int  straggler    = hasStraggler ? d[n - 1] : 0;
    if (hasStraggler)
        d.pop_back();

    std::deque<int> main_chain;
    std::deque<int> pend_chain;

    for (size_t i = 0; i < d.size(); i += 2)
    {
        if (d[i] > d[i + 1])
        {
            main_chain.push_back(d[i]);
            pend_chain.push_back(d[i + 1]);
        }
        else
        {
            main_chain.push_back(d[i + 1]);
            pend_chain.push_back(d[i]);
        }
    }

    sortDeque(main_chain);
    insertionDeque(main_chain, pend_chain);

    if (hasStraggler)
    {
        std::deque<int>::iterator pos =
            std::lower_bound(main_chain.begin(), main_chain.end(), straggler);
        main_chain.insert(pos, straggler);
    }

    d = main_chain;
}

// ─── printSequence ───────────────────────────────────────────────────────────

void PmergeMe::printSequence(const std::vector<int> &v) const
{
    for (size_t i = 0; i < v.size(); i++)
    {
        if (i > 0) std::cout << " ";
        std::cout << v[i];
    }
    std::cout << std::endl;
}

// ─── run ─────────────────────────────────────────────────────────────────────
// Executa o algoritmo nos dois contêineres, mede o tempo e imprime o resultado.
// clock() é C++98 compatible — retorna ticks de CPU.

void PmergeMe::run()
{
    // Imprime sequência original (antes de ordenar)
    std::cout << "Before: ";
    printSequence(_vec);

    // ── Ordena com vector ──
    struct timeval startVec, endVec;
    gettimeofday(&startVec, NULL);
    sortVector(_vec);
    gettimeofday(&endVec, NULL);
    double timeVec = (endVec.tv_sec - startVec.tv_sec) * 1000000.0
                   + (endVec.tv_usec - startVec.tv_usec);

    // ── Ordena com deque ──
    struct timeval startDeq, endDeq;
    gettimeofday(&startDeq, NULL);
    sortDeque(_deq);
    gettimeofday(&endDeq, NULL);
    double timeDeq = (endDeq.tv_sec - startDeq.tv_sec) * 1000000.0
                   + (endDeq.tv_usec - startDeq.tv_usec);

    // Imprime sequência ordenada
    std::cout << "After:  ";
    printSequence(_vec);

    // Imprime tempos com precisão de 5 casas decimais (microsegundos)
    std::cout << std::fixed << std::setprecision(5);
    std::cout << "Time to process a range of " << _vec.size()
              << " elements with std::vector : " << timeVec << " us" << std::endl;
    std::cout << "Time to process a range of " << _deq.size()
              << " elements with std::deque  : " << timeDeq << " us" << std::endl;
}