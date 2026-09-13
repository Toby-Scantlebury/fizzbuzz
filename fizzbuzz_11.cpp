// Step 11: Replace eager storage with a bounded lazy transform
#include <iostream>
#include <string>

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

template<std::ranges::input_range Range>
void print_range(std::ostream& out, Range&& values)
{
    for (auto&& value : values)
        out << value << '\n';
}

auto fizzbuzz_sequence()
{
    return std::views::iota(1, 21) | std::views::transform(fizzbuzz_value);
}

int main()
{
    print_range(std::cout, fizzbuzz_sequence());
}
