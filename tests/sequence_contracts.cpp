// Exercise the final coroutine implementation without sharing code between lessons.
#define FIZZBUZZ_NO_MAIN
#include "../fizzbuzz_22.cpp"
#undef FIZZBUZZ_NO_MAIN

#include <memory>
#include <sstream>
#include <stdexcept>
#include <vector>

static_assert(std::ranges::input_range<Generator<int>>);
static_assert(std::ranges::view<Generator<int>>);
static_assert(!std::ranges::forward_range<Generator<int>>);
static_assert(!std::copy_constructible<Generator<int>>);

void require(bool condition)
{
    if (!condition) throw std::runtime_error("sequence contract failed");
}

template<std::ranges::input_range Range>
auto collect(Range&& range)
{
    std::vector<std::ranges::range_value_t<Range>> result;
    for (const auto& value : range) result.push_back(value);
    return result;
}

Generator<int> failing()
{
    co_yield 7;
    throw std::runtime_error("producer failure");
}

Generator<int> keep_alive(std::shared_ptr<int> token)
{
    co_yield *token;
    co_yield *token + 1;
}

int main()
{
    auto identity = [](int i) { return i; };
    auto sum = [](int a, int b) { return a + b; };
    auto empty = generate_mapped(std::vector<int>{}, identity);
    require(collect(empty).empty());

    // Both the container and callable are temporaries owned by the frame.
    auto owned = generate_mapped(std::vector<int>{1, 2, 3},
                                [factor = 2](int i) { return i * factor; });
    require(collect(owned) == std::vector<int>({2, 4, 6}));

    auto unequal = zip_with(std::vector<int>{1, 2, 3}, std::vector<int>{10}, sum);
    require(collect(unequal) == std::vector<int>({11}));
    auto empty_left = zip_with(std::vector<int>{}, std::vector<int>{10}, sum);
    auto empty_right = zip_with(std::vector<int>{10}, std::vector<int>{}, sum);
    require(collect(empty_left).empty());
    require(collect(empty_right).empty());

    int evaluations = 0;
    auto lazy = generate_mapped(std::views::iota(1, 100),
        [&evaluations](int i) { ++evaluations; return i; });
    require(evaluations == 0);
    {
        auto it = lazy.begin();
        require(*it == 1 && evaluations == 1);
        ++it;
        require(*it == 2 && evaluations == 2);
    }

    auto token = std::make_shared<int>(42);
    std::weak_ptr<int> weak = token;
    {
        auto first = keep_alive(token);
        token.reset();
        auto moved = std::move(first);
        require(!weak.expired());
        require(*moved.begin() == 42);
        // Destroy while suspended, without exhausting the producer.
    }
    require(weak.expired());

    auto failure = failing();
    auto it = failure.begin();
    require(*it == 7);
    bool caught = false;
    try { ++it; }
    catch (const std::runtime_error& e) { caught = std::string(e.what()) == "producer failure"; }
    require(caught);

    Accumulator value(15);
    value.concat("");
    require(value.render() == "15");
    value.concat("Fizz");
    value.concat("Buzz");
    require(value.render() == "Fizzbuzz");
    require(value.render() == "Fizzbuzz"); // Rendering does not consume the value.

    auto baz = generate_mapped(std::views::iota(1, 7),
                              Conditional{DivisibleBy{2}, Word{"Baz"}});
    auto flick = generate_mapped(std::views::iota(1, 7),
                                Conditional{DivisibleBy{3}, Word{"Flick"}});
    auto combined = zip_with(std::move(baz), std::move(flick),
        [](std::string a, const std::string& b) { append_word(a, b); return a; });
    require(collect(combined) == std::vector<std::string>({"", "Baz", "Flick", "Baz", "", "Bazflick"}));

    std::ostringstream out;
    print_range(out, std::vector<int>{1, 2});
    require(out.str() == "1\n2\n");
}
