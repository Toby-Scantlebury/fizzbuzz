// Step 2: A single ordered if-else chain
#include <iostream>

int main()
{
    for (int i = 1; i <= 20; ++i)
    {
        if (i % 15 == 0)
            std::cout << "Fizzbuzz\n";
        else if (i % 3 == 0)
            std::cout << "Fizz\n";
        else if (i % 5 == 0)
            std::cout << "Buzz\n";
        else
            std::cout << i << '\n';
    }
}
