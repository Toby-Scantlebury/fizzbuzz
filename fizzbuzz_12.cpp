// Step 12: Separate sequence generation from prefix selection
#include <iostream>
#include <string>

#include <ranges>
#include <limits>

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
    // Bounded by the index type: no signed overflow in an "infinite" loop.
    return std::views::iota(1, std::numeric_limits<int>::max())
         | std::views::transform(fizzbuzz_value);
}

int main()
{
    print_range(std::cout, fizzbuzz_sequence() | std::views::take(20));
}
