#include <string_view>
#include <vector>
#include <iostream>
#include <string>
#include <cstdint>

using u64 = std::uint64_t;

int bf(std::string_view code)
{
    struct Instruction { char cmd; std::size_t arg; };
    std::vector<Instruction> instructions;

    // parse to instructions
    {
        std::vector<std::size_t> starts;
        for (auto c : code) switch (c)
        {
        case '>': case '<':
        case '+': case '-':
        case '.': case ',':
            instructions.emplace_back(Instruction{ c, 0 });
            break;
        case '[':
            starts.emplace_back(instructions.size());
            instructions.emplace_back(Instruction{ c, 0 });
            break;
        case ']':
            {
            if (starts.empty())
                { std::cerr << "syntax error: ] missing [\n"; return -1; }
            const auto start = *starts.rbegin(); starts.pop_back();
            instructions[start].arg = instructions.size();
            instructions.emplace_back(Instruction{ c, start });
            break;
            }
        }

        if (false == starts.empty())
            { std::cerr << "syntax error: [ missing ]\n"; return -1; }
    }

    // execute instructions
    {
        std::size_t ip = 0;
        std::size_t dp = 0;
        std::vector<char> data; data.resize(30000, 0);
        auto get = [&]() -> char
        {
            if (dp < data.size()) { return data[dp]; }
            else { return 0; }
        };
        auto set = [&](char d)
        {
            if (dp >= data.size()) { data.resize(dp + 1, 0); }
            data[dp] = d;
        };
        while (ip < instructions.size())
        {
            switch (instructions[ip].cmd)
            {
            case '>': dp += 1; ip += 1; break;
            case '<': dp -= 1; ip += 1; break;
            case '+': set(get() + 1); ip += 1; break;
            case '-': set(get() - 1); ip += 1; break;
            case '.': std::cout << get(); ip += 1; break;
            case ',': set(std::cin.get()); ip += 1; break;
            case '[': if (get() == 0) { ip = instructions[ip].arg; } else { ip += 1; } break;
            case ']': if (get() != 0) { ip = instructions[ip].arg; } else { ip += 1; } break;
            }
        }
    }

    return 0;
}

int main(int argc, char** argv)
{
    return bf("++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.");
}
