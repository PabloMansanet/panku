#pragma once

namespace TupleManipulation {

   template<int... Is>
   struct sequence { };

   template<int N, int... Is>
   struct gen_sequence : gen_sequence<N - 1, N - 1, Is...> { };

   template<int... Is>
   struct gen_sequence<0, Is...> : sequence<Is...> { };

   template<typename T, typename F, int... Is>
   void for_each(T&& t, F f, sequence<Is...>)
   {
      auto l = { (f(std::get<Is>(t)), 0)... };
   }

   template<typename... Ts, typename F>
   void for_each_in_tuple(std::tuple<Ts...> & t, F f)
   {
       for_each(t, f, gen_sequence<sizeof...(Ts)>());
   }
}

