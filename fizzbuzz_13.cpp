// Step 13: Inject the source range as well as the transformation
#include <iostream>
#include <string>

#include <ranges>
#include <utility>

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

template<std::ranges::viewable_range Range, class ValueAt>
auto map_sequence(Range&& indices, ValueAt value_at)
{
    return std::views::all(std::forward<Range>(indices))
         | std::views::transform(std::move(value_at));
}

template<std::ranges::input_range Range>
void print_range(std::ostream& out, Range&& values)
{
    for (auto&& value : values)
        out << value << '\n';
}

int main()
{
    print_range(std::cout, map_sequence(std::views::iota(1, 21), fizzbuzz_value));
}
