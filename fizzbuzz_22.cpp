// Step 22: Combine independently generated sequences with generic zip-with
#include <iostream>
#include <string>

#include <ranges>
#include <utility>

#include <cctype>
#include <string_view>
#include <variant>

#include <coroutine>
#include <cstddef>
#include <exception>
#include <functional>
#include <iterator>
#include <limits>
#include <optional>
#include <type_traits>

// Move-only, single-pass, synchronous generator. Values are owned by the frame.
// Calling begin() more than once is outside this generator's contract.
template<class T>
class Generator : public std::ranges::view_interface<Generator<T>>
{
public:
    struct promise_type;
    using Handle = std::coroutine_handle<promise_type>;

    struct promise_type
    {
        std::optional<T> current;
        std::exception_ptr error;
        Generator get_return_object() { return Generator(Handle::from_promise(*this)); }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(T value)
        {
            current.emplace(std::move(value));
            return {};
        }
        void return_void() noexcept {}
        void unhandled_exception() noexcept { error = std::current_exception(); }
    };

    struct Iterator
    {
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using iterator_concept = std::input_iterator_tag;
        Handle handle{};

        const T& operator*() const { return *handle.promise().current; }
        Iterator& operator++()
        {
            handle.resume();
            if (handle.promise().error)
                std::rethrow_exception(handle.promise().error);
            return *this;
        }
        void operator++(int) { ++*this; }
        bool operator==(std::default_sentinel_t) const
        {
            return !handle || handle.done();
        }
    };

    Generator() = default;
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;
    Generator(Generator&& other) noexcept : handle_(std::exchange(other.handle_, {})) {}
    Generator& operator=(Generator&& other) noexcept
    {
        if (this != &other)
        {
            if (handle_) handle_.destroy();
            handle_ = std::exchange(other.handle_, {});
        }
        return *this;
    }
    ~Generator() { if (handle_) handle_.destroy(); }
    Iterator begin()
    {
        Iterator it{handle_};
        if (handle_ && !handle_.done()) ++it;
        return it;
    }
    std::default_sentinel_t end() const { return {}; }

private:
    explicit Generator(Handle handle) : handle_(handle) {}
    Handle handle_{};
};

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

template<std::ranges::input_range Range, class Function>
Generator<std::remove_cvref_t<std::invoke_result_t<Function&, std::ranges::range_reference_t<Range>>>>
generate_mapped(Range source, Function function)
{
    for (auto&& value : source)
        co_yield std::invoke(function, value);
}

template<std::ranges::input_range Left, std::ranges::input_range Right, class Combine>
Generator<std::remove_cvref_t<std::invoke_result_t<Combine&,
    std::ranges::range_reference_t<Left>, std::ranges::range_reference_t<Right>>>>
zip_with(Left left, Right right, Combine combine)
{
    auto l = std::ranges::begin(left);
    auto r = std::ranges::begin(right);
    const auto lend = std::ranges::end(left);
    const auto rend = std::ranges::end(right);
    // Stop at the shorter source, including empty sources.
    while (l != lend && r != rend)
    {
        co_yield std::invoke(combine, *l, *r);
        ++l;
        ++r;
    }
}

template<std::ranges::input_range Range>
void print_range(std::ostream& out, Range&& values)
{
    for (auto&& value : values)
        out << value << '\n';
}

#ifndef FIZZBUZZ_NO_MAIN
int main()
{
    auto fizz = generate_mapped(std::views::iota(1, 21),
                               Conditional{DivisibleBy{3}, Word{"Fizz"}});
    auto buzz = generate_mapped(std::views::iota(1, 21),
                               Conditional{DivisibleBy{5}, Word{"Buzz"}});
    auto words = zip_with(std::move(fizz), std::move(buzz),
        [](const std::string& a, const std::string& b)
        {
            auto result = a;
            append_word(result, b);
            return result;
        });
    auto values = zip_with(std::views::iota(1, 21), std::move(words),
        [](int i, const std::string& word)
        {
            Accumulator result(i);
            result.concat(word);
            return result;
        });
    // Transfer the move-only source into the presentation view.
    print_range(std::cout, std::move(values) | std::views::transform([](const auto& value) { return value.render(); }));
    return 0;
}
#endif
