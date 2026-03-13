#ifndef RPN_HPP
# define RPN_HPP

# include <iostream>
# include <sstream>
# include <string>
# include <stack>

// std::stack foi escolhido porque:
// 1. RPN é um algoritmo naturalmente baseado em pilha (LIFO)
// 2. Empilhamos números ao encontrá-los
// 3. Ao encontrar um operador, desempilhamos os 2 últimos, operamos, reempilhamos
// 4. std::stack expõe exatamente as operações necessárias: push, pop, top, empty
// 5. É diferente de std::map (usado no ex00) — regra do módulo respeitada

class RPN
{
    public:
        RPN();
        RPN(const RPN &other);
        RPN &operator=(const RPN &other);
        ~RPN();

        // Avalia a expressão e imprime o resultado (ou erro no stderr)
        void evaluate(const std::string &expression);

    private:
        std::stack<long> _stack;

        bool isOperator(const std::string &token) const;
        bool applyOperator(const std::string &op);
};

#endif