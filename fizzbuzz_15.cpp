// Step 15: Generalize divisibility tests to runtime predicates
#include <iostream>
#include <string>

#include <ranges>
#include <utility>

#include <array>
#include <cctype>
#include <string_view>

#include <functional>

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

std::string evaluate(int i, const auto& rules)
{
    std::string text;
    for (const auto& rule : rules)
        if (rule.matches(i))
            append_word(text, rule.word);
    return text.empty() ? std::to_string(i) : text;
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
    print_range(std::cout, values);
}
