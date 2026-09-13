// Step 4: Extract the complete operation
#include <iostream>

void print_fizzbuzz()
{
    for (int i = 1; i <= 20; ++i)
    {
        const bool fizz = i % 3 == 0;
        const bool buzz = i % 5 == 0;
        if (fizz && buzz)
            std::cout << "Fizzbuzz\n";
        else if (fizz)
            std::cout << "Fizz\n";
        else if (buzz)
            std::cout << "Buzz\n";
        else
            std::cout << i << '\n';
    }
}

int main()
{
    print_fizzbuzz();
}
