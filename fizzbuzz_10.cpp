// Step 10: Consume any input range rather than a particular container
#include <iostream>
#include <string>

#include <vector>
#include <ranges>

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

template<std::ranges::input_range Range>
void print_range(std::ostream& out, Range&& values)
{
    for (auto&& value : values)
        out << value << '\n';
}

int main()
{
    print_range(std::cout, make_sequence(20, fizzbuzz_value));
}
