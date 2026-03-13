#include "RPN.hpp"

// ─── Forma Canônica Ortodoxa ─────────────────────────────────────────────────

RPN::RPN() {}

RPN::RPN(const RPN &other) : _stack(other._stack) {}

RPN &RPN::operator=(const RPN &other)
{
    if (this != &other)
        _stack = other._stack;
    return *this;
}

RPN::~RPN() {}

// ─── isOperator ──────────────────────────────────────────────────────────────
// Verifica se o token é um dos quatro operadores suportados.

bool RPN::isOperator(const std::string &token) const
{
    return (token == "+" || token == "-" || token == "*" || token == "/");
}

// ─── applyOperator ───────────────────────────────────────────────────────────
// Desempilha os dois operandos, aplica a operação e empilha o resultado.
//
// ATENÇÃO NA ORDEM: em RPN "3 2 -" significa 3 - 2, NÃO 2 - 3.
// O topo da pilha é o operando DIREITO (b), o segundo é o ESQUERDO (a).
//
// Exemplo passo a passo de "7 7 * 7 -":
//   token "7"  → stack: [7]
//   token "7"  → stack: [7, 7]
//   token "*"  → b=7, a=7 → 7*7=49 → stack: [49]
//   token "7"  → stack: [49, 7]
//   token "-"  → b=7, a=49 → 49-7=42 → stack: [42]
//   resultado: 42

bool RPN::applyOperator(const std::string &op)
{
    // Precisamos de pelo menos 2 operandos na pilha
    if (_stack.size() < 2)
    {
        std::cerr << "Error" << std::endl;
        return false;
    }

    long b = _stack.top(); _stack.pop(); // operando direito
    long a = _stack.top(); _stack.pop(); // operando esquerdo

    long result = 0;

    if (op == "+")
        result = a + b;
    else if (op == "-")
        result = a - b;
    else if (op == "*")
        result = a * b;
    else if (op == "/")
    {
        if (b == 0)
        {
            std::cerr << "Error: division by zero." << std::endl;
            return false;
        }
        result = a / b;
    }

    _stack.push(result);
    return true;
}

// ─── evaluate ────────────────────────────────────────────────────────────────
// Algoritmo principal de avaliação RPN:
//
//   1. Percorre a expressão token por token (separados por espaço)
//   2. Se o token for um número (< 10 conforme o subject) → empilha
//   3. Se for operador → aplica sobre os 2 topos da pilha
//   4. Se for qualquer outra coisa → erro
//   5. No final, a pilha deve ter exatamente 1 elemento: o resultado
//
// O subject diz que os números passados como argumento são sempre < 10,
// mas o resultado pode ser qualquer valor — por isso usamos long.

void RPN::evaluate(const std::string &expression)
{
    // Limpa a pilha para reutilização segura do objeto
    while (!_stack.empty())
        _stack.pop();

    std::istringstream iss(expression);
    std::string token;

    while (iss >> token)
    {
        if (isOperator(token))
        {
            // Tenta aplicar o operador; se falhar, aborta
            if (!applyOperator(token))
                return;
        }
        else
        {
            // Tenta converter o token para número
            // O subject garante que inputs válidos são dígitos únicos (< 10),
            // mas verificamos se é realmente um número inteiro válido
            std::istringstream tokenStream(token);
            long num;
            tokenStream >> num;

            // Se a conversão falhou ou sobrou caractere (ex: "1a", "(1")
            if (tokenStream.fail() || !tokenStream.eof())
            {
                std::cerr << "Error" << std::endl;
                return;
            }

            _stack.push(num);
        }
    }

    // Expressão válida deve deixar exatamente 1 valor na pilha
    if (_stack.size() != 1)
    {
        std::cerr << "Error" << std::endl;
        return;
    }

    std::cout << _stack.top() << std::endl;
}