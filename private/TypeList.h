#include <type_traits>
#include <tuple>

#pragma once

// Code adapted from geoyar's article at:
// https://www.codeproject.com/Articles/1077852/TypeLists-and-a-TypeList-Toolbox-via-Variadic-Temp

namespace TypeList
{

  template <typename T>
  struct type_to_type
  {
    using orig = T;
    type_to_type() {}
  };

///////////////////////////////////////////////////////////////////////////////
/// NullType and Sizeof to assign size zero to NullType
/// NullType definition
  class NullType
  {};

/// Helper: assigns size 0 to NullType, calculates sizeof(T) for others types

  template <typename T>
  struct Sizeof
  {
    static constexpr size_t value = std::is_same<T, NullType>::value ? 0 : sizeof(T);
  };

//-----------------------------------------------------------------------------
/// TypeList definition; defines size similar to  C++ STL containers size

  template <typename... Ts>
  struct type_list
  {
    using type = type_list;
    static constexpr size_t size() noexcept { return sizeof...(Ts); }
  };

  using type_list_emptylist = type_list<>;

//-----------------------------------------------------------------------------
/// Is this type a type_list?

  template <typename T>
  struct is_type_list
  {
  private:
    using yes = const char(&&)[1];
    using no  = const char(&&)[2];

    template <typename... Ts>
    static yes Test(type_to_type<type_list<Ts...>>) { return {}; }

    static no Test(...) { return {}; }

  public:
    using type = std::integral_constant<bool, sizeof(Test(type_to_type<T>())) == 1>;
    using value_type = typename type::value_type;
    static constexpr value_type value = type::value;
  };

  //---------------------------------------------------------------------------
  ///Test whether all list's elements are TypeLists: used as a switch

  template <typename List>    // Is called if List is not type_list
  struct is_listoflists       // or on empty list
  {
    using type = std::integral_constant<bool, false>;
    using value_type = typename type::value_type;
    static constexpr value_type value = type::value;
  };

  template <typename H, typename... ListEls>
  struct is_listoflists<type_list<H, ListEls...>>
  {
  private:
    static constexpr bool bLst = is_type_list<H>::value;
  public:
    using type = typename std::conditional < bLst,
      typename is_listoflists<type_list<ListEls...>>::type, std::integral_constant<bool, false >> ::type;
    using value_type = typename type::value_type;            // Goes there only not type_list,stops
    static constexpr value_type value = type::value;
  };

  template <typename ListEl>                                 // Got to the end of original list
  struct is_listoflists<type_list<ListEl>>
  {
  private:
    static constexpr bool bLst = is_type_list<ListEl>::value;
  public:
    using type = std::integral_constant<bool, bLst>;
    using value_type = typename type::value_type;
    static constexpr value_type value = type::value;
  };

//-----------------------------------------------------------------------------
/// Access an element by an index

  template <size_t idx, class List>
  struct type_list_type_at_impl;

  template <typename T, typename... Ts>  ///> End of search, type is found
  struct type_list_type_at_impl < 0, type_list<T, Ts...> >
  {
    using type = T;
  };

/// Recursion
  template <size_t idx, typename T, typename...Ts>
  struct type_list_type_at_impl <idx, type_list<T, Ts...>>
  {
    using type = typename type_list_type_at_impl<idx - 1, type_list<Ts...>>::type;
  };

/// Wrapper
  template <size_t idx, class List>
  struct type_list_type_at;

  template <size_t idx, typename... Ts>
  struct type_list_type_at<idx, type_list<Ts...>>
  {
  private:
    static_assert(sizeof...(Ts) > idx,
      "type_list_type_at: Index out of bounds or called on empty list");
  public:
    using type = typename type_list_type_at_impl<idx, type_list<Ts...>>::type;

  };

//-----------------------------------------------------------------------------
/// Search a TypeList for a first occurrence of the type T

/// Implementation: has index as a template parameter
  template <size_t idx, typename T, class List>
  struct type_list_index_of_impl;

  template <size_t idx, typename T> /// The type T is not in the list
  struct type_list_index_of_impl <idx, T, type_list<>>
  {
    using type = std::integral_constant<int, -1>;
  };

  template <size_t idx, typename T, typename... Ts>    ///> The type is found
  struct type_list_index_of_impl <idx, T, type_list<T, Ts...>>
  {
    using type = std::integral_constant<int, idx>;
  };

  template <size_t idx, typename T, typename H, typename... Ts>  ///> Recursion
  struct type_list_index_of_impl <idx, T, type_list<H, Ts...>>
  {
    using type = typename type_list_index_of_impl<idx + 1, T, type_list<Ts...>>::type;
  };

/// Wrapping to supply initial index 0
  template <typename T, class List>
  struct type_list_index_of;

/// Specializing for idx >= 0
  template <typename T, typename... Ts>
  struct type_list_index_of<T, type_list<Ts...>>
  {
    using type = typename type_list_index_of_impl<0, T, type_list<Ts...>>::type;
    using value_type = typename type::value_type;
    static constexpr value_type value = type::value;
  };

//-----------------------------------------------------------------------------
/// Push front and push back

  template <typename H, class List>
  struct type_list_push_front; ;

  template <typename H, typename... Ts>
  struct type_list_push_front<H, type_list<Ts...>>
  {
    using type = type_list<H, Ts...>;
  };

  template <typename T, class List>
  struct type_list_push_back;

  template <typename T, typename... Ts>
  struct type_list_push_back<T, type_list<Ts...>>
  {
    using type = type_list<Ts..., T>;
  };
//---------------------------------------------------------------------------
/// Concatenate TypeLists

/// Implementation
  template <class ResList, class OrigLists>
  struct type_list_concat_lists_impl;

  template <typename... Rs, typename... Ts>                     // Last OrigList
  struct type_list_concat_lists_impl<type_list<Rs...>, type_list<Ts...>>
  {
    using type = type_list<Rs..., Ts...>;
  };

  template <typename... Rs, typename... Ts, typename... Rests>  // Recursion
  struct type_list_concat_lists_impl<type_list<Rs...>, type_list<type_list<Ts...>, Rests...>>
  {
    using type = typename type_list_concat_lists_impl<type_list<Rs..., Ts...>, type_list<Rests...>>::type;
  };

/// Flatten a list of lists (convert it to a plain list) - is used as standalone and in a wrapper
  template <class ListOfLists>
  struct type_list_flatten_list;

  template <typename... Lists>
  struct type_list_flatten_list<type_list<Lists...>>
  {
  private:
    static_assert(is_listoflists<type_list<Lists...>>::value,
                                 "type_list_flatten: Argument is not a type_list of lists");
  public:
    using type = typename type_list_concat_lists_impl<type_list<>, type_list<Lists...>>::type;
  };

  template <typename... Lists>
  struct type_list_concat_lists
  {
  private:
    using temp_list = type_list<Lists...>;
    static_assert(is_listoflists<temp_list>::value, "Not all arguments are lists");
  public:
    using type = typename type_list_flatten_list<temp_list>::type;
  };

//-------------------------------------------------------------------------------
/// Convert type_list to tuple and back (From Peter Dimov)

  template <class Src, template <typename...> class Trg>
  struct convert_impl;

  template <template <typename... > class Src, typename... Ts, template <typename...> class Trg>
  struct convert_impl<Src<Ts...>, Trg>
  {
    using type = Trg<Ts...>;
  };

  template <class Src, template <typename...> class Trg>
  using convert = typename convert_impl<Src, Trg>::type;

}

