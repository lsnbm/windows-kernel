#pragma once

// 内核模式下没有标准库，我们需要自己实现所需的功能
namespace std_kernel {

    // 定义 integer_sequence，用于在编译时存储一串整数
    template <typename T, T... Ints>
    struct integer_sequence {
        using value_type = T;
        static constexpr size_t size() noexcept { return sizeof...(Ints); }
    };

    template <size_t... Ints>
    using index_sequence = integer_sequence<size_t, Ints...>;

    // 使用一种更兼容的递归方式来生成 integer_sequence
    template <size_t N, size_t... Next>
    struct index_sequence_builder : index_sequence_builder<N - 1, N - 1, Next...> {};

    // 递归的终止条件
    template <size_t... Next>
    struct index_sequence_builder<0, Next...> {
        using type = index_sequence<Next...>;
    };

    template <size_t N>
    using make_index_sequence = typename index_sequence_builder<N>::type;

} // namespace std_kernel

// 使用更稳定的编译时种子来替换 __TIME__ ---
// __COUNTER__ 是一个每次使用时都会自增的宏，完全符合 constexpr 的要求
constexpr int time_seed_kernel = 'n' + 't' + 'o' + 's' + 'k' + 'r' + 'n' + 'l' + '.' + 'e' + 'x' + 'e' + __COUNTER__;

template <typename T, size_t N>
class XorStringKernel {
public:
    constexpr XorStringKernel(const T(&str)[N])
        : XorStringKernel(str, std_kernel::make_index_sequence<N>{}) {
    }

    const T* decrypt() const {
        for (size_t i = 0; i < N; ++i) {
            unsigned int key_part1 = static_cast<unsigned int>(i);
            unsigned int key_part2 = static_cast<unsigned int>(time_seed_kernel);
            mutable_data[i] = static_cast<T>(data[i] ^ (key_part1 + (key_part2 % 256)));
        }
        return mutable_data;
    }

private:
    template <size_t... I>
    constexpr XorStringKernel(const T(&str)[N], std_kernel::index_sequence<I...>)
        : data{ (T)(str[I] ^ ((unsigned int)I + ((unsigned int)time_seed_kernel % 256)))... }, mutable_data{} {
    }

    const T data[N];
    mutable T mutable_data[N];
};

// 宏定义保持不变
#define OBFUSCATE(s) ([]() { \
    constexpr XorStringKernel str(s); \
    return str; \
}())
