
#include <algorithm>
#include <string>
#include <variant>
#include <iostream>
#include <ranges>

#include <coro/coro.hpp>

//////////////////////////////////////////////////
//////////// General Util/helpers ////////////////
//////////////////////////////////////////////////

void tolower(std::string &data)
{
    std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c){ return std::tolower(c); });
}

template<class... Ts>
struct overloads : Ts... { using Ts::operator()...; };

template<size_t N>
struct String_Literal
{
    constexpr String_Literal(const char (&str)[N])
    {
        std::copy_n(str, N, value);
    }

    const char * operator()() { return value; }
    
    char value[N];
};


//////////////////////////////////////////////////
//////////// Number Sequence Utils ///////////////
//////////////////////////////////////////////////

template <class ConditionT, class TrueT, class FalseT>
struct Number_Sequence_View
{
    auto operator()(int i) const { return ConditionT::check(i) ? TrueT::show(i) : FalseT::show(i); }
};

struct AlwaysTrue { static constexpr bool check(int) { return true; } };
struct Identity { static constexpr int show(int i) { return i; } };

template <int Mod> struct Condition_True_On_Modulo { static constexpr bool check(int i) { return (i % Mod) == 0; } };
template <String_Literal Str> struct Show_String { static constexpr std::string show(int) { return Str.value; } };

template <class AccumulatorT, class... Seq>
void accumulate_index(AccumulatorT &acc, int idx, const Seq&... seqs)
{
    (acc.concat(seqs(idx)), ... );
};

template <class AccumulatorT, class... Seq>
coro::generator<AccumulatorT> sequence_concatenator(const Seq&... seqs)
{
    for (int idx = 1; ; idx++)
    {
        AccumulatorT acc;
        accumulate_index(acc, idx, seqs...);
        co_yield acc;
    }
};

//////////////////////////////////////////////////
//////////////// Range Helpers ///////////////////
//////////////////////////////////////////////////

void print_to_stdout(std::ranges::input_range auto&& range)
{
    std::ranges::copy(range, std::ostream_iterator<std::string>(std::cout, "\n"));
}

auto take_n_from_range(std::ranges::input_range auto&& range, int count)
{
    return std::move(range) | std::views::take(count);
}

void print_n_to_stdout(std::ranges::input_range auto&& gen, int count)
{
    print_to_stdout(take_n_from_range(gen, count));
}

//////////////////////////////////////////////////
//////////// FizzBuzz Specialisation /////////////
//////////////////////////////////////////////////
 
namespace fizzbuzz
{

struct Accumulator
{
public:
    void concat(int i)
    {
        data = i;
    }
    void concat(const std::string &b)
    {
        std::visit(overloads
        {
            [&](int){ if (!b.empty()) data = b; },
            [&](std::string &s){ auto c = b; tolower(c); s += c; }
        }, data);
    }
    void concat(Accumulator &other)
    {
        std::visit(overloads
        {
            [&](int i){ concat(i); },
            [&](std::string &s){ concat(s); }
        }, other.data);
    }

    std::string extract()
    {
        return std::visit(overloads
        {
            [](int i){ return std::to_string(i); },
            [](std::string &s){ return std::move(s); }
        }, data);
    }

private:
    std::variant<int, std::string> data;
};

using Integers = Number_Sequence_View<AlwaysTrue, Identity, Identity>;
using Fizz = Number_Sequence_View<Condition_True_On_Modulo<3>, Show_String<"Fizz">, Show_String<"">>;
using Buzz = Number_Sequence_View<Condition_True_On_Modulo<5>, Show_String<"Buzz">, Show_String<"">>;

coro::generator<Accumulator> sequence()
{
    Integers integers;
    Fizz fizzes;
    Buzz buzzes;
    for (auto value : sequence_concatenator<Accumulator>(integers, fizzes, buzzes))
        co_yield value;
};

auto sequence_as_string()
{
    return sequence() | std::views::transform([](auto &x) { return x.extract(); });
};

void print_n(int count)
{
    print_n_to_stdout(sequence_as_string(), count);
}

} // namespace fizzbuzz

//////////////////////////////////////////////////
//////////// BazFlick Specialisation /////////////
//////////////////////////////////////////////////

namespace bazflick
{

using Accumulator = fizzbuzz::Accumulator;

struct DoubleInteger { static constexpr int show(int i) { return 2 * i; } };

using DoubleEvenIntegers = Number_Sequence_View<Condition_True_On_Modulo<2>, DoubleInteger, Identity>;
using BazOrDouble = Number_Sequence_View<Condition_True_On_Modulo<6>, Show_String<"Baz">, Show_String<"">>;
using Flick = Number_Sequence_View<Condition_True_On_Modulo<4>, Show_String<"Flick">, Show_String<"">>;

coro::generator<Accumulator> sequence()
{
    DoubleEvenIntegers integers;
    BazOrDouble fizzes;
    Flick buzzes;
    for (auto value : sequence_concatenator<Accumulator>(integers, fizzes, buzzes))
        co_yield value;
};

auto sequence_as_string()
{
    return sequence() | std::views::transform([](auto &x) { return x.extract(); });
};

void print_n(int count)
{
    print_n_to_stdout(sequence_as_string(), count);
}

} // namespace bazflick

namespace combined
{

using Accumulator = fizzbuzz::Accumulator;

coro::generator<Accumulator> combined_sequence(coro::generator<Accumulator>& s1, coro::generator<Accumulator>& s2)
{
    // ranges::zip not available
    auto it1 = s1.begin();
    auto it2 = s2.begin();
    while (1)
    {
        auto acc1 = *it1;
        auto acc2 = *it2;
        acc1.concat(acc2);
        co_yield acc1;
        ++it1;
        ++it2;
    }
}

coro::generator<Accumulator> sequence()
{
    auto s1 = bazflick::sequence();
    auto s2 = fizzbuzz::sequence();
    return combined_sequence(s1, s2);
}

auto sequence_as_string()
{
    return sequence() | std::views::transform([](auto &x) { return x.extract(); });
}

void print_n(int count)
{
    print_n_to_stdout(sequence_as_string(), count);
}

} // namespace combined

int main()
{
    bazflick::print_n(20);
    fizzbuzz::print_n(20);
    combined::print_n(20);

    return 0;
}