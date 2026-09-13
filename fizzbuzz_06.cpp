// Step 6: Inject the output stream and retain a stdout convenience function
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

void print_fizzbuzz(std::ostream& out)
{
    for (int i = 1; i <= 20; ++i)
        out << fizzbuzz_value(i) << '\n';
}

void print_fizzbuzz()
{
    print_fizzbuzz(std::cout);
}

int main()
{
    print_fizzbuzz();
}
