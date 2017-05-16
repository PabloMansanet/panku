#pragma once

namespace TupleManipulation {

   template<int... Integers>
   struct sequence { };

   template<int N, int... Integers>
   struct gen_sequence : gen_sequence<N - 1, N - 1, Integers...> { };

   template<int... Integers>
   struct gen_sequence<0, Integers...> : sequence<Integers...> { };

   template<typename Tuple, typename Functor, int... Integers>
   void for_each(Tuple&& tuple, Functor functor, sequence<Integers...>)
   {
      auto l = { (functor(std::get<Integers>(tuple)), 0)... };
   }

   template<typename... Types, typename Functor>
   void for_each_in_tuple(std::tuple<Types...> & tuple, Functor functor)
   {
       for_each(tuple, functor, gen_sequence<sizeof...(Types)>());
   }
}

