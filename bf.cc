#include <string_view>
#include <vector>
#include <iostream>
#include <string>
#include <array>
#include <cstdint>

using u64 = std::uint64_t;

constexpr std::size_t bytes_per_ref = 8;


int bf(std::string_view code)
{
    std::vector<char> instructions;
    auto set_ref = [&](std::size_t start, u64 to)
    {
        const auto offset = start + 1;
        instructions[offset + 0] = static_cast<char>((to >> 0 ) & 0xFF);
        instructions[offset + 1] = static_cast<char>((to >> 8 ) & 0xFF);
        instructions[offset + 2] = static_cast<char>((to >> 16) & 0xFF);
        instructions[offset + 3] = static_cast<char>((to >> 24) & 0xFF);
        instructions[offset + 4] = static_cast<char>((to >> 32) & 0xFF);
        instructions[offset + 5] = static_cast<char>((to >> 40) & 0xFF);
        instructions[offset + 6] = static_cast<char>((to >> 48) & 0xFF);
        instructions[offset + 7] = static_cast<char>((to >> 56) & 0xFF);
    };

    auto get_ref = [&](std::size_t start) -> u64
    {
        const auto offset = start + 1;
        return
            static_cast<uint64_t>(instructions[offset + 0]) << 0  |
            static_cast<uint64_t>(instructions[offset + 1]) << 8  |
            static_cast<uint64_t>(instructions[offset + 2]) << 16 |
            static_cast<uint64_t>(instructions[offset + 3]) << 24 |
            static_cast<uint64_t>(instructions[offset + 4]) << 32 |
            static_cast<uint64_t>(instructions[offset + 5]) << 40 |
            static_cast<uint64_t>(instructions[offset + 6]) << 48 |
            static_cast<uint64_t>(instructions[offset + 7]) << 56 ;
    };

    // parse to instructions
    {
        std::vector<std::size_t> starts;
        for (auto c : code) switch (c)
        {
        case '>': case '<':
        case '+': case '-':
        case '.': case ',':
            instructions.emplace_back(c);
            break;
        case '[':
            starts.emplace_back(instructions.size());
            instructions.emplace_back(c);
            for(int i=0; i< bytes_per_ref; i+=1){ instructions.emplace_back(0); }
            break;
        case ']':
            {
            if (starts.empty())
                { std::cerr << "syntax error: ] missing [\n"; return -1; }
            const auto start = *starts.rbegin(); starts.pop_back();
            const auto self = instructions.size();
            set_ref(start, self);
            instructions.emplace_back(c);
            for (int i = 0; i < bytes_per_ref; i += 1) { instructions.emplace_back(0); }
            set_ref(self, start);
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
            switch (instructions[ip])
            {
            case '>': dp += 1; ip += 1; break;
            case '<': dp -= 1; ip += 1; break;
            case '+': set(get() + 1); ip += 1; break;
            case '-': set(get() - 1); ip += 1; break;
            case '.': std::cout << get(); ip += 1; break;
            case ',': set(std::cin.get()); ip += 1; break;
            case '[': if (get() == 0) { ip = get_ref(ip); } else { ip += 1 + bytes_per_ref; } break;
            case ']': if (get() != 0) { ip = get_ref(ip); } else { ip += 1 + bytes_per_ref; } break;
            }
        }
    }

    return 0;
}

int main(int argc, char** argv)
{
    return bf("++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.");
}
