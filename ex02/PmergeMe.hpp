#ifndef PMERGEME_HPP
# define PMERGEME_HPP

# include <iostream>
# include <sstream>
# include <string>
# include <vector>
# include <deque>
# include <ctime>
# include <iomanip>
# include <sys/time.h>

// ─── Contêineres usados ───────────────────────────────────────────────────────
//
// std::vector  — array dinâmico, acesso O(1) por índice, inserção O(n) no meio
// std::deque   — fila dupla, inserção/remoção O(1) em ambas extremidades
//
// Ambos são diferentes de std::map (ex00) e std::stack (ex01).
// O ex02 exige DOIS contêineres diferentes para o mesmo algoritmo,
// permitindo comparar a performance entre eles.
//
// ─── Algoritmo Ford-Johnson (Merge-Insert Sort) ───────────────────────────────
//
// Objetivo: ordenar com o menor número possível de COMPARAÇÕES.
// É teoreticamente ótimo para sequências pequenas/médias.
//
// Passos:
//   1. Dividir a sequência em pares. O elemento maior de cada par forma
//      a "cadeia principal" (main chain). O menor fica como "pendente".
//   2. Ordenar recursivamente a cadeia principal pelos seus maiores.
//   3. Inserir os elementos pendentes na cadeia principal usando
//      busca binária, na ordem de Jacobsthal (minimiza comparações).
//
// Sequência de Jacobsthal: 0, 1, 1, 3, 5, 11, 21, 43, 85, 171, ...
// A ordem de inserção baseada em Jacobsthal garante que cada inserção
// binária use no máximo o mesmo número de comparações que a anterior.

class PmergeMe
{
    public:
        PmergeMe();
        PmergeMe(const PmergeMe &other);
        PmergeMe &operator=(const PmergeMe &other);
        ~PmergeMe();

        void    parseArgs(int argc, char **argv);
        void    run();

    private:
        std::vector<int> _vec;
        std::deque<int>  _deq;

        // ── Ford-Johnson para std::vector ──
        void    sortVector(std::vector<int> &v);
        void    insertionVector(std::vector<int> &main, std::vector<int> &pend);

        // ── Ford-Johnson para std::deque ──
        void    sortDeque(std::deque<int> &d);
        void    insertionDeque(std::deque<int> &main, std::deque<int> &pend);

        // ── Utilitários ──
        void            printSequence(const std::vector<int> &v) const;
        std::vector<int> jacobsthalSequence(int n) const;
};

#endif