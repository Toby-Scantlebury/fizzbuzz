// Step 1: Nested divisibility tests in main
#include <iostream>

int main()
{
    for (int i = 1; i <= 20; ++i)
    {
        if (i % 3 == 0)
        {
            if (i % 5 == 0)
                std::cout << "Fizzbuzz\n";
            else
                std::cout << "Fizz\n";
        }
        else
        {
            if (i % 5 == 0)
                std::cout << "Buzz\n";
            else
                std::cout << i << '\n';
        }
    }
}
