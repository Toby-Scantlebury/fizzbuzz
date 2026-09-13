// Step 9: Materialize values independently of their consumer
#include <iostream>
#include <string>

#include <vector>

std::string fizzbuzz_value(int i)
{
    const bool fizz = i % 3 == 0;
    const bool buzz = i % 5 == 0;
    if (fizz && buzz)
        return "Fizzbuzz";
    else if (fizz)
        return "Fizz";
    else if (buzz)
        return "Buzz";
    else
        return std::to_string(i);
}

template<class ValueAt>
std::vector<std::string> make_sequence(int count, ValueAt value_at)
{
    std::vector<std::string> result;
    for (int offset = 0; offset < count; ++offset)
        result.push_back(value_at(offset + 1));
    return result;
}

void print_sequence(std::ostream& out, const std::vector<std::string>& values)
{
    for (const auto& value : values)
        out << value << '\n';
}

int main()
{
    print_sequence(std::cout, make_sequence(20, fizzbuzz_value));
}
