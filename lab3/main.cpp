#include <iostream>
#include <complex>

namespace bashar{
    template<int Exp, typename T = double, typename enable = void>
    struct pow{
        constexpr T operator()(T bas){
            pow<Exp -1, T> next;
            return next(bas) * bas;
        }
    };

    template<typename T>
    struct pow<0, T>{
        constexpr T operator()(T bas){
            return bas == (T)0 ? (T)0 :(T)1;
        }
    };

    template<int Exp, typename T>
    struct pow<Exp, T, typename std::enable_if_t<Exp < 0>> {
        constexpr T operator()(T bas) {
            pow<Exp + 1, T> next;
            return next(bas) * 1 / bas;
        }
    };
}

int main(){
    bashar::pow<2> result;
    std::cout<<result(2) << std::endl;
}
