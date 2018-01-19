#pragma once

namespace TupleManipulation {

   template<int... Integers>
   struct sequence { };

   template<int N, int... Integers>
   struct gen_sequence : public gen_sequence<N - 1, N - 1, Integers...> { };

   template<int... Integers>
   struct gen_sequence<0, Integers...> : public sequence<Integers...> { };

   template<typename Tuple, typename Functor, int... Integers>
   void for_each(Tuple&& tuple, Functor functor, sequence<Integers...>)
   {
      auto l = { (functor(std::get<Integers>(tuple)), 0)... };
      (void)l;
   }

   template<typename... Types, typename Functor>
   void for_each_in_tuple(std::tuple<Types...> & tuple, Functor functor)
   {
       for_each(tuple, functor, gen_sequence<sizeof...(Types)>());
   }

   template <typename T, typename Tuple>
   struct has_type;

   template <typename T>
   struct has_type<T, std::tuple<>> 
   {
      static constexpr bool value = false;
   };

   template <typename T, typename U, typename... Ts>
   struct has_type<T, std::tuple<U, Ts...>> : public has_type<T, std::tuple<Ts...>> {};

   template <typename T, typename... Ts>
   struct has_type<T, std::tuple<T, Ts...>> : public std::true_type 
   {
      static constexpr bool value = true;
   };
}

