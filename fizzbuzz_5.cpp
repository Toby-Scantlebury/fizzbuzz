// Step 5: Separate value calculation from iteration and output
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

void print_fizzbuzz()
{
    for (int i = 1; i <= 20; ++i)
        std::cout << fizzbuzz_value(i) << '\n';
}

int main()
{
    print_fizzbuzz();
}
