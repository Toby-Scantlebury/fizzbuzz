// Step 8: Inject the value-producing operation
#include <iostream>
#include <string>

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
void print_sequence(std::ostream& out, int count, ValueAt value_at)
{
    for (int offset = 0; offset < count; ++offset)
        out << value_at(offset + 1) << '\n';
}

int main()
{
    print_sequence(std::cout, 20, fizzbuzz_value);
}
