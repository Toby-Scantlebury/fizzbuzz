// Step 17: Keep numbers and words typed until the presentation boundary
#include <iostream>
#include <string>

#include <ranges>
#include <utility>

#include <array>
#include <cctype>
#include <string_view>

#include <functional>
#include <variant>

void append_word(std::string& text, std::string_view word)
{
    if (text.empty())
        text = word;
    else
        for (unsigned char c : word)
            text += static_cast<char>(std::tolower(c));
}

struct Rule
{
    std::function<bool(int)> matches;
    std::string word;
};

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

Accumulator evaluate(int i, const auto& rules)
{
    Accumulator result(i);
    for (const auto& rule : rules)
        if (rule.matches(i))
            result.concat(rule.word);
    return result;
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
    const std::array rules{Rule{[](int i) { return i % 3 == 0; }, "Fizz"},
                           Rule{[](int i) { return i % 5 == 0; }, "Buzz"}};
    auto values = map_sequence(std::views::iota(1, 21),
        [rules](int i) { return evaluate(i, rules); });
    print_range(std::cout, values | std::views::transform([](const auto& value) { return value.render(); }));
}
