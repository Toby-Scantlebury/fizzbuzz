# FizzBuzz, one abstraction at a time

Every `fizzbuzz_N.cpp` is a complete, independent C++20 program with its own
`main`. No shared project headers, generated sources, or external dependencies
are needed. All examples print indices 1 through 20, one per line, using `Fizz`,
`Buzz`, and `Fizzbuzz` (the spelling used by the original).

```sh
cmake -S . -B build
cmake --build build -j 2
ctest --test-dir build --output-on-failure
./build/fizzbuzz_23
# Build just one lesson:
cmake --build build --target fizzbuzz_6
# Or compile any example directly:
c++ -std=c++20 fizzbuzz_23.cpp -o /tmp/fizzbuzz_23
```

## The progression

Each file repeats the definitions it needs so it can be read and built alone.
Read in numerical order; the repetition is intentional.

| Step | New abstraction / responsibility |
| --- | --- |
| [1](fizzbuzz_01.cpp) | Nested divisibility tests in main |
| [2](fizzbuzz_02.cpp) | A single ordered if-else chain |
| [3](fizzbuzz_03.cpp) | Name the two independent conditions |
| [4](fizzbuzz_04.cpp) | Extract the complete operation |
| [5](fizzbuzz_05.cpp) | Separate value calculation from iteration and output |
| [6](fizzbuzz_06.cpp) | Inject the output stream and retain a stdout convenience function |
| [7](fizzbuzz_07.cpp) | Let the caller choose the sequence length |
| [8](fizzbuzz_08.cpp) | Inject the value-producing operation |
| [9](fizzbuzz_09.cpp) | Materialize values independently of their consumer |
| [10](fizzbuzz_10.cpp) | Consume any input range rather than a particular container |
| [11](fizzbuzz_11.cpp) | Replace eager storage with a bounded lazy transform |
| [12](fizzbuzz_12.cpp) | Separate sequence generation from prefix selection |
| [13](fizzbuzz_13.cpp) | Inject the source range as well as the transformation |
| [14](fizzbuzz_14.cpp) | Represent independent divisibility rules as data |
| [15](fizzbuzz_15.cpp) | Generalize divisibility tests to runtime predicates |
| [16](fizzbuzz_16.cpp) | Inject the fallback presentation policy |
| [17](fizzbuzz_17.cpp) | Keep numbers and words typed until the presentation boundary |
| [18](fizzbuzz_18.cpp) | Compose heterogeneous predicate/projection policies without type erasure |
| [19](fizzbuzz_19.cpp) | Make the accumulation algebra a policy too |
| [20](fizzbuzz_20.cpp) | Produce values with a coroutine while retaining range consumers |
| [21](fizzbuzz_21.cpp) | Generalize the coroutine to any owned input range |
| [22](fizzbuzz_22.cpp) | Combine independently generated sequences with generic zip-with |
| [23](fizzbuzz_23.cpp) | Package domain policies as reusable lazy pipeline adaptors |

## Design choices and limits

Steps 1–6 follow the requested structure. Step 7 makes length configurable;
nonpositive counts produce no output. Steps 9–10 deliberately demonstrate eager
storage before step 11 removes it. Laziness means a transformation runs when
an element is read, and can run again on repeated dereferences.

Steps 14–16 support ordered runtime rules. Divisors must be positive. Step 15
uses `std::function` to store different predicates in one homogeneous array;
step 18 replaces this with a heterogeneous template parameter pack. Empty words
have no effect. The first word retains its capitalization, and later words are
lowercased, matching the original's Fizzbuzz/Bazflick accumulation convention.
Rules and strings are captured by value where a returned operation needs to own
them. Step 14's string views refer to string literals.

Step 17 delays numeric formatting until consumption. Step 19 makes the result
policy replaceable: it must be constructible from an index and accept each rule
result through `concat`. This allows other accumulation behavior without changing
the sequence machinery. A `Conditional` independently selects a predicate and a
word projection, so Baz/Flick rules can use the same machinery.

Steps 20–22 include a small synchronous coroutine generator to keep each file
self-contained on C++20 compilers. It is move-only and single-pass: call `begin`
once, do not increment an exhausted iterator, and do not keep references across
increments. Exceptions are propagated to the consumer and early destruction
releases the coroutine frame. These examples yield owned values, not references.
Coroutine arguments are taken by value; a passed view can still borrow its
underlying storage, which must outlive iteration. `zip_with` stops at the shorter
input. As with ordinary zip iteration, advancing it may pull an extra value from
one input when the other ends.

Step 23 returns to standard transform views and adds reusable, pipeable domain
adaptors. Coroutines are useful for stateful production and combining streams;
a pure per-index mapping needs only standard views. This is the final abstraction
boundary: sources, rules, accumulation, formatting, prefix selection, and output
can vary independently. More layers without a new responsibility would obscure
the example. The custom adaptor supports `range | adaptor`, not composition of
two adaptor objects without a range.
