// Step 7: Let the caller choose the sequence length
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

void print_fizzbuzz(std::ostream& out, int count)
{
    for (int i = 0; i < count; ++i)
        out << fizzbuzz_value(i + 1) << '\n';
}

void print_fizzbuzz(int count)
{
    print_fizzbuzz(std::cout, count);
}

int main()
{
    print_fizzbuzz(20);
}
