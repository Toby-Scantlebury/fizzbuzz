// Step 19: Make the accumulation algebra a policy too
#include <iostream>
#include <string>

#include <ranges>
#include <utility>

#include <cctype>
#include <string_view>
#include <variant>

void append_word(std::string& text, std::string_view word)
{
    if (text.empty())
        text = word;
    else
        for (unsigned char c : word)
            text += static_cast<char>(std::tolower(c));
}

class Accumulator
{
    std::variant<int, std::string> data_;

public:
    explicit Accumulator(int index) : data_(index) {}

    void concat(std::string_view word)
    {
        if (word.empty())
            return;
        if (std::holds_alternative<int>(data_))
            data_ = std::string(word);
        else
            append_word(std::get<std::string>(data_), word);
    }

    std::string render() const
    {
        if (const auto* number = std::get_if<int>(&data_))
            return std::to_string(*number);
        return std::get<std::string>(data_);
    }
};

template<class Predicate, class Projection>
struct Conditional
{
    Predicate matches;
    Projection project;

    std::string operator()(int i) const
    {
        return matches(i) ? project(i) : std::string{};
    }
};

struct DivisibleBy
{
    int divisor; // Precondition: positive.
    bool operator()(int i) const { return i % divisor == 0; }
};

struct Word
{
    std::string text;
    std::string operator()(int) const { return text; }
};

template<class Result, class... Rules>
auto compose_rules(Rules... rules)
{
    // Own every policy: the returned callable can outlive this factory.
    return [... rules = std::move(rules)](int i)
    {
        Result result(i);
        (result.concat(rules(i)), ...);
        return result;
    };
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
    auto value_at = compose_rules<Accumulator>(Conditional{DivisibleBy{3}, Word{"Fizz"}},
                                  Conditional{DivisibleBy{5}, Word{"Buzz"}});
    auto values = map_sequence(std::views::iota(1, 21), std::move(value_at));
    print_range(std::cout, values | std::views::transform([](const auto& value) { return value.render(); }));
}
