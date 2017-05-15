
#include <type_traits>
#include <tuple>

#pragma once

namespace TypeList
{
//-----------------------------------------------------------------------------
/// Helper to use with is_type_list, is_iseq (borrowed)

  template <typename T>
  struct type_to_type
  {
    using orig = T;
    type_to_type() {}
  };

/////////////////////////////////////////////////////////////////////////////
/// Helpers to work with integer_seqyense

//-----------------------------------------------------------------------------
/// Is the class integer-sequence

  template <typename T>
  struct is_iseq
  {
  private:
    using yes = char(&)[1];
    using no  = char(&)[2];

    template <typename U, U... Is>
    static yes Test(type_to_type<std::integer_sequence<U, Is...>>) {}

    static no Test(...) {}

  public:
    using type = std::integral_constant<bool, sizeof(Test(type_to_type<T>())) == 1>;
    using value_type = typename type::value_type;
    static constexpr value_type value = type::value;
  };

//-----------------------------------------------------------------------------
/// Get a number at the index 

  template <size_t idx, class ISeq>
  struct iseq_nmb_at_impl;

  template <typename U, U head, U... Is>  /// End of search, number is found
  struct iseq_nmb_at_impl < 0, std::integer_sequence<U, head, Is...> >
  {
    using type = std::integral_constant<U, head>;
  };

/// Recursion
  template <size_t idx, typename U, U head, U...Is>
  struct iseq_nmb_at_impl<idx, std::integer_sequence<U, head, Is...>>
  {
    using type = typename iseq_nmb_at_impl<idx - 1, std::integer_sequence<U, Is...>>::type;
  };

/// Wrapper; tests the index, has added 'value_type' and 'value' members
  template <size_t idx, class ISeg>
  struct iseq_nmb_at;

  template <size_t idx, typename U, U... Is>
  struct iseq_nmb_at<idx, std::integer_sequence<U, Is...>>
  {
  private:
    static_assert(std::integer_sequence<U, Is...>::size() > idx,
      "iseq_nmb_at: Index is out of sequence bounds");
  public:
    using type = typename iseq_nmb_at_impl<idx, std::integer_sequence<U, Is...>>::type; 
    using value_type = typename type::value_type;
    static constexpr value_type value = type::value;
  };

//-----------------------------------------------------------------------------
/// Get the index of a number

  template <size_t idx, typename U,U nmb, class ISeq>
  struct iseq_index_of_impl;

  template <size_t idx, typename U, U nmb>                  /// Number not the in sequence
  struct iseq_index_of_impl<idx, U, nmb, std::integer_sequence<U>>
  {
    using type = std::integral_constant<int, -1>;        /// Stop recursion
  };

  template <size_t idx, typename U, U nmb, U... Ns>        /// Number found
  struct iseq_index_of_impl<idx, U, nmb, std::integer_sequence<U, nmb, Ns...>>
  {
    using type = std::integral_constant<int, idx>;       
  };

  template <size_t idx, typename U, U nmb, U head, U... Ns>
  struct iseq_index_of_impl<idx, U, nmb, std::integer_sequence<U, head, Ns...>>
  {
    using type = typename iseq_index_of_impl<idx + 1, U, nmb, std::integer_sequence<U, Ns...>>::type;
  };

/// Wrapper to test arguments, supply starting index 0, and add members 'value_type' and 'value'

  template <typename U, U nmb, class ISeq>
  struct iseq_index_of
  {
  private:
    static_assert(is_iseq<ISeq>::value, 
      "iseq_index_of: The class supplied is not std::integer_sequence");
    using data_type = typename ISeq::value_type;       
    static_assert(std::is_same<U, data_type>::value, 
      "iseq_index_of: Number must be the same type as the sequence");
  public:
    using type = typename iseq_index_of_impl<0, data_type, nmb, ISeq>::type;
    using value_type = typename type::value_type;
    static constexpr value_type value = type::value;
  };

//-----------------------------------------------------------------------------
/// Remove some number from a sequence

  template <typename U, U nmb, class IntSeq1, class IntSeq2>
  struct iseq_remove_nmb_impl;

  template <typename U, U nmb, U... Ns>      /// Called on empty sequence or the number not found
  struct iseq_remove_nmb_impl<U, nmb, std::integer_sequence<U, Ns...>, std::integer_sequence<U>>
  {
    using type = std::integer_sequence<U, Ns...>;
  };

  template <typename U, U nmb, U... N1s, U... N2s>  // Number found and removed
  struct iseq_remove_nmb_impl<U, nmb, std::integer_sequence<U, N1s...>, std::integer_sequence<U, nmb, N2s...>>
  {
    using type = std::integer_sequence<U, N1s..., N2s...>;
  };

  template <typename U, U nmb, U head, U... N1s, U... N2s>
  struct iseq_remove_nmb_impl <U, nmb, std::integer_sequence<U, N1s...>, std::integer_sequence<U, head, N2s...>>
  {
    using type = typename iseq_remove_nmb_impl<U, nmb, std::integer_sequence<U, N1s..., head>, 
                                                                  std::integer_sequence<U, N2s...>>::type;
  };

/// Wrapper

  template <typename U, U nmb, class ISeq>
  struct iseq_remove_nmb
  {
  private:
    static_assert ( is_iseq<ISeq>::value, 
      "iseq_remove_nmb: Supplied class is not integral_sequence");
    using data_type = typename ISeq::value_type;
    static_assert(std::is_same<U, data_type>::value, 
      "iseq_remove_nmb: Number must be of the type of sequence numbers");
  public:
    using type =
      typename iseq_remove_nmb_impl<data_type, nmb, std::integer_sequence<data_type>, ISeq>::type;
  };

//-----------------------------------------------------------------------------
/// Push a number back:
  template <typename T, T nmb, typename U, class ISeg>
  struct iseq_push_back_impl;

  template <typename T, T nmb, typename U, U... Ns>
  struct iseq_push_back_impl<T, nmb, U, std::integer_sequence<U, Ns...>>
  {
    using type = std::integer_sequence<U, Ns..., nmb>;
  };

/// Wrapper to check template arguments
  template <typename T, T nmb, class ISeq>
  struct iseq_push_back
  {
  private:
    static_assert(is_iseq<ISeq>::value, 
      "iseq_push_back: Supplied class is not std::integer_sequence");
    using data_type = typename ISeq::value_type;
    static_assert(std::is_same<T, data_type>::value, 
      "iseq_push_back: Number must be of the type of sequence numbers");
  public:
    using type = typename iseq_push_back_impl<T, nmb, data_type, ISeq>::type;
  };

//----------------------------------------------------------------------------
/// Get the sum of std::integer_sequence members

  template <typename U, U sum, class IntSeq>
  struct iseq_sum_impl;

  template <typename U, U sum>                               /// End of recursion
  struct iseq_sum_impl <U, sum, std::integer_sequence< U>>
  {
    using type = typename std::integral_constant<U, sum>;
  };

  template <typename U, U sum, U head, U... Ns>             /// Recursion
  struct iseq_sum_impl < U, sum, std::integer_sequence<U, head, Ns...>>
  {
    using type = typename iseq_sum_impl<U, sum + head, std::integer_sequence<U, Ns...>>::type;
  };

  /// Wrapper to check template arguments and supply initial sum = 0
  template <class ISeq>
  struct iseq_sum
  {
  private:
    static_assert(is_iseq<ISeq>::value, "iseq_sum: Supplied class is not inteter_sequence");
    using data_type = typename ISeq::value_type;
  public:
    using type = typename iseq_sum_impl<data_type, 0, ISeq>::type;
    using value_type = typename type::value_type;
    static constexpr value_type value = type::value;
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
/// Is the list empty? Has 'type', 'value_type' and 'value' members

  template <class List>
  struct is_type_list_empty
  {
    using type = std::false_type; // integral_constant<bool, false>;
    using value_type = typename type::value_type;
    static constexpr value_type value = type::value;
  };

/// Specialization for an empty list

  template <>
  struct is_type_list_empty<type_list<>>
  {
    using type = std::true_type; // integral_constant<bool, true>;
    using value_type = type::value_type;
    static constexpr value_type value = type::value;
  };

//-----------------------------------------------------------------------------
/// Is this type a type_list?

  template <typename T>
  struct is_type_list
  {
  private:
    using yes = char(&)[1];
    using no  = char(&)[2];

    template <typename... Ts>
    static yes Test(type_to_type<type_list<Ts...>>) {}

    static no Test(...) {}

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
/// Find the first element not equal to argument T

  template <size_t idx, typename T, class List>
  struct type_list_index_of_not_impl;

  template <size_t idx, typename T>             // All list's elements are T
  struct type_list_index_of_not_impl<idx, T, type_list<>>
  {
    using type = std::integral_constant<int, -1>;
  };

  template <size_t idx, typename T, typename H, typename... Ts>
  struct type_list_index_of_not_impl <idx, T, type_list<H, Ts...>>
  {
    using type = typename std::conditional < std::is_same<T, H>::value,
      typename type_list_index_of_not_impl<idx + 1, T, type_list<Ts...>>::type,
      std::integral_constant<int, idx >> ::type;
  };

/// Wrapper
  template <typename T, class List>
  struct type_list_index_of_not;

  template <typename T, typename... Ts>
  struct type_list_index_of_not<T, type_list<Ts...>>
  {
    using type = typename type_list_index_of_not_impl<0, T, type_list<Ts...>>::type;
    using value_type = typename type::value_type;
    static constexpr value_type value = type::value;
  };

//-----------------------------------------------------------------------------
/// Access to the front and the back of the list

  template <class Lists>
  struct type_list_front; 

  template <typename... Ts>
  struct type_list_front<type_list<Ts...>>
  {
  private:
    static_assert(sizeof...(Ts) > 0, "type_list_front: Called on empty list");
  public:
    using type = typename type_list_type_at<0, type_list<Ts...>>::type;
  };

  template <class List>
  struct type_list_back;

  template <typename... Ts>
  struct type_list_back<type_list<Ts...>>
  {
  private:
    static_assert(sizeof...(Ts) > 0, "type_list_back: Called on empty list");
  public:
    using type = typename type_list_type_at< (sizeof...(Ts)) - 1, type_list<Ts...>>::type;
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

//-----------------------------------------------------------------------------
/// Erase an element from the list

  template <typename T, class List>
  struct type_list_erase_type; 

  template <typename T>
  struct type_list_erase_type<T, type_list<>>
  {
    using type = type_list<>;
  };

  template <typename T, typename... Ts>
  struct type_list_erase_type <T, type_list<T, Ts...>>
  {
    using type = type_list<Ts...>;
  };

  template <typename T, typename H, typename... Ts>
  struct type_list_erase_type<T, type_list<H, Ts...>>
  {
    using type =
      typename type_list_push_front<H, typename type_list_erase_type<T, type_list<Ts...>>::type>::type;
  };

//-----------------------------------------------------------------------------
/// Erase an element from a TypeList by its index

  template <size_t idx, class List>
  struct type_list_erase_at_impl; 

  template <typename T, typename... Ts>
  struct type_list_erase_at_impl<0, type_list<T, Ts...>>
  {
    using type = type_list<Ts...>;
  };

  template <size_t idx, typename H, typename... Ts>
  struct type_list_erase_at_impl<idx, type_list<H, Ts...>>
  {
    using type =
      typename type_list_push_front < H, typename type_list_erase_at_impl<idx - 1, type_list<Ts...>>::type>::type;
  };

/// Wrapper to assert index is in range
  template <size_t idx, class List>
  struct type_list_erase_at;

  template <size_t idx, typename... Ts>
  struct type_list_erase_at<idx, type_list<Ts...>>
  {
  private:
    static_assert(sizeof...(Ts) > idx, "type_list_erase_at: Index out of bounds");
  public:
    using type = typename type_list_erase_at_impl<idx, type_list<Ts...>>::type;
  };

//-----------------------------------------------------------------------------
/// Pop front, this can be called on empty list

  template <class List>
  struct type_list_pop_front;

  template <typename... Ts>
  struct type_list_pop_front<type_list<Ts...>>
  {
    using type = typename std::conditional < sizeof...(Ts) == 0, type_list<>,
                   typename  type_list_erase_at<0, type_list<Ts...>>::type> ::type;
  };

//-----------------------------------------------------------------------------
/// Pop back, allows an empty list

  template <class List>
  struct type_list_pop_back;

  template <typename... Ts>
  struct type_list_pop_back<type_list<Ts...>>
  {
    using type = typename std::conditional<sizeof...(Ts) == 0, type_list<>,
             typename type_list_erase_at<sizeof...(Ts)-1, type_list<Ts...>>::type>::type;
  };

//-----------------------------------------------------------------------------
/// Erase all occurrence of type

  template <typename T, class List>
  struct type_list_erase_all;

  template <typename T>
  struct type_list_erase_all<T, type_list<>>
  {
    using type = type_list<>;
  };

  template <typename T, typename... Ts>
  struct type_list_erase_all<T, type_list<T, Ts...>>
  {
    using type = typename type_list_erase_all<T, type_list<Ts...>>::type;
  };

  template <typename T, typename H, typename... Ts>
  struct type_list_erase_all<T, type_list<H, Ts...>>
  {
    using type = typename type_list_push_front < H, typename type_list_erase_all<T, type_list<Ts...>>::type>::type;
  };

//-----------------------------------------------------------------------------
/// Erase all elements of given type from a list of lists
/// Never use it as a standalone meta function, use type_list_deeperase_all instead

  template <typename T, class List>
  struct type_list_erase_all_ll
  {
    using type = type_list<>;
  };

  template <typename T, typename... Hs, typename... Ts>
  struct type_list_erase_all_ll<T, type_list<type_list<Hs...>, Ts...>>
  {
  private:
    using el_cleaned = typename type_list_erase_all<T, type_list<Hs...>>::type;
  public:
    using type =
      typename type_list_push_front<el_cleaned, typename type_list_erase_all_ll<T, type_list<Ts...>>::type>::type;
  };

//-----------------------------------------------------------------------------
/// Wrapper for deep erasure (inside each of list's elements)

  template <typename T, class List>
  struct type_list_deeperase_all;

  template <typename T, typename... ListEls>
  struct type_list_deeperase_all<T, type_list<ListEls...>>
  {
  private:
    static constexpr bool bLL = is_listoflists <type_list<ListEls...>>::value;
  public:
    using type = typename std::conditional < bLL,
      typename type_list_erase_all_ll<T, type_list<ListEls...>>::type,
      typename type_list_erase_all<T, type_list<ListEls...>>::type>::type;
  };

//-----------------------------------------------------------------------------
/// Erase all duplicates from the list

  template <class List>
  struct type_list_erase_dupl; 

  template <>
  struct type_list_erase_dupl<type_list<>>
  {
    using type = type_list<>;
  };

  template <typename H, typename... Ts>
  struct type_list_erase_dupl<type_list<H, Ts...>>
  {
  private:
    using unique_t = typename type_list_erase_dupl<type_list<Ts...>>::type;
    using new_t = typename type_list_erase_type<H, unique_t>::type;
  public:
    using type = typename type_list_push_front<H, new_t>::type;
  };

//---------------------------------------------------------------------------
/// Erase duplicates in all elements of list of lists
/// Not use it as a standalone meta function; use type_list_deeperase_dupl instead

  template <class List>
  struct type_list_erase_dupl_ll
  {
    using type = type_list<>;
  };
  
  template <typename... Hs, typename... ListEls>
  struct type_list_erase_dupl_ll<type_list<type_list<Hs...>, ListEls...>>
  {
  private:
    using lst_clean = typename type_list_erase_dupl < type_list<Hs...>>::type;
  public:
    using type =
      typename type_list_push_front<lst_clean,
      typename type_list_erase_dupl_ll<type_list<ListEls...>>::type>::type;;
  };

//---------------------------------------------------------------------------
/// Wrapper for a deep erasure of duplicates

  template <class List>
  struct type_list_deeperase_dupl;

  template <typename... ListEls>
  struct type_list_deeperase_dupl<type_list<ListEls...>>
  {
  private:
    static constexpr bool bLL = is_listoflists<type_list<ListEls...>>::value;
  public:
    using type = typename std::conditional < bLL,
      typename type_list_erase_dupl_ll<type_list<ListEls...>>::type,
      typename type_list_erase_dupl<type_list<ListEls...>>::type>::type;
  };

//---------------------------------------------------------------------------
/// Erase first k elements from the list

  template <size_t k, class List>
  struct type_list_erase_firsts; //{};

  template <size_t k>
  struct type_list_erase_firsts<k, type_list<>>
  {
    using type = type_list<>;
  };

  template <typename... Ts>
  struct type_list_erase_firsts < 1, type_list<Ts...>>
  {
    using type = typename type_list_pop_front < type_list<Ts...>>::type;
  };

  template <size_t k, typename... Ts>
  struct type_list_erase_firsts<k, type_list<Ts...>>
  {
  private:
    static_assert(k > 0, "Wrong number of items to erase (k must be > 0)");
  public:
    using type = typename type_list_erase_firsts<k - 1, typename type_list_pop_front<type_list<Ts...>>::type>::type;
  };

//-----------------------------------------------------------------------------
// Erase last k elements

  template <size_t k, class List>
  struct type_list_erase_lasts;

  template <size_t k>
  struct type_list_erase_lasts<k, type_list<>>
  {
    using type = type_list<>;
  };

  template <typename... Ts>
  struct type_list_erase_lasts<1, type_list<Ts...>>
  {
    using type = typename type_list_pop_back<type_list<Ts...>>::type;
  };

  template <size_t k, typename... Ts>
  struct type_list_erase_lasts<k, type_list<Ts...>>
  {
  private:
    static_assert(k > 0, "Wrong number of items to erase (k must be > 0)");
  public:
    using type =
      typename type_list_erase_lasts<k - 1, typename type_list_pop_back<type_list<Ts...>>::type>::type;
  };

//-----------------------------------------------------------------------------
/// Replace the first occurrence of the type in the TypeList
/// R - replacement for the type T

  template <typename R, typename T, class List>
  struct type_list_replace_first; ;

  template <typename R, typename T>
  struct type_list_replace_first<R, T, type_list<>>
  {
    using type = type_list<>;
  };

  template <typename R, typename T, typename H, typename... Ts>
  struct type_list_replace_first<R, T, type_list<H, Ts...>>
  {
    using type = typename std::conditional<std::is_same<T, H>::value,
      type_list<R, Ts...>,
      typename type_list_push_front<H, typename type_list_replace_first<R, T, type_list<Ts...>>::type>::type>::type;
  };

//-----------------------------------------------------------------------------
/// Replace a type at index; R is a replacement

  template <size_t idx, typename R, class List>
  struct type_list_replace_at_impl;

  template <typename R, typename H, typename... Ts>   // Index reached, type replaced
  struct type_list_replace_at_impl<0, R, type_list<H, Ts...>>
  {
    using type = type_list<R, Ts...>;
  };

  template <size_t idx, typename R, typename H, typename... Ts>
  struct type_list_replace_at_impl<idx, R, type_list<H, Ts...>>  // Recursion
  {
    using type =
      typename type_list_push_front < H, typename type_list_replace_at_impl<idx - 1, R, type_list<Ts...>>::type>::type;
  };

/// Wrapper to check the index
  template <size_t idx, typename R, class List>
  struct type_list_replace_at;

  template <size_t idx, typename R, typename... Ts>
  struct type_list_replace_at<idx, R, type_list<Ts...>>
  {
  private:
    static_assert(sizeof...(Ts) > idx, "type_list_replace_at: Index is out of bounds");
  public:
    using type = typename type_list_replace_at_impl<idx, R, type_list<Ts...>>::type;
  };

//-----------------------------------------------------------------------------
/// Replace all occurrences of a given type in a type_list; R is a replacement for type T

  template <typename R, typename T, class List>
  struct type_list_replace_all;

  template <typename R, typename T>
  struct type_list_replace_all<R, T, type_list<>>
  {
    using type = type_list<>;
  };

  template <typename R, typename T, typename... Ts>
  struct type_list_replace_all<R, T, type_list<T, Ts...>>
  {
    using type = typename type_list_push_front<R, typename type_list_replace_all<R, T, type_list<Ts...>>::type>::type;
  };

  template <typename R, typename T, typename H, typename... Ts>
  struct type_list_replace_all<R, T, type_list<H, Ts...>>
  {
    using type = typename type_list_push_front<H, typename type_list_replace_all<R, T, type_list<Ts...>>::type>::type;
  };

//-----------------------------------------------------------------------------
/// Replace all occurrences of a type in a list of lists elements
/// Do not use it as a standalone meta function; use type_list_deepreplace_all instead

  template <typename R, typename T, class List>
  struct type_list_replace_all_ll
  {
    using type = type_list<>;
  };

  template <typename R, typename T, typename... Hs, typename... ListEls>
  struct type_list_replace_all_ll<R, T, type_list<type_list<Hs...>, ListEls...>>
  {
  private:
    using lst_replaced = typename type_list_replace_all<R, T, type_list<Hs...>>::type;
  public:
    using type =
      typename type_list_push_front < lst_replaced, 
      typename type_list_replace_all_ll<R, T, type_list<ListEls...>>::type>::type; 
  };

//-----------------------------------------------------------------------------
/// Wrapper to deep replacement

  template <typename R, typename T, class List>
  struct type_list_deepreplace_all;

  template <typename R, typename T, typename... ListEls>
  struct type_list_deepreplace_all<R, T, type_list<ListEls...>>
  {
  private:
    static constexpr bool bLL = is_listoflists <type_list<ListEls...>>::value;
  public:
    using type = typename std::conditional<bLL,
      typename type_list_replace_all_ll<R, T, type_list<ListEls...>>::type,
      typename type_list_replace_all<R, T, type_list<ListEls...>>::type>::type;
  };

//-----------------------------------------------------------------------------
/// Count a type in the list

  template <size_t k, typename T, class List>
  struct type_list_count_type_impl;

  template <size_t k, typename T >
  struct type_list_count_type_impl<k, T, type_list<>>
  {
    using type = std::integral_constant<size_t, k>;
  };

  template <size_t k, typename T, typename H, typename... Ts>
  struct type_list_count_type_impl<k, T, type_list<H, Ts...>>
  {
  private:
    static constexpr size_t count = std::is_same<T, H>::value ? k + 1 : k;
  public:
    using type = typename type_list_count_type_impl<count, T, type_list<Ts...>>::type;
  };

/// Wrapper
  template <typename T, class Loist>
  struct type_list_count_type;

  template <typename T, typename... Ts>
  struct type_list_count_type<T, type_list<Ts...>>
  {
    using type = typename type_list_count_type_impl<0, T, type_list<Ts...>>::type;
    using value_type = typename type::value_type;
    static constexpr value_type value = type::value;
  };

//---------------------------------------------------------------------------
/// Count types in the list of lists
/// Do not use it as a standalone meta function; use type_list_deepcount_type instead

  template <typename T, class ISeq, class List>
  struct type_list_count_type_ll
  {
    using type = std::integer_sequence<size_t>;
  };

  template <typename T, size_t... Rs, typename... Hs >
  struct type_list_count_type_ll<T, std::integer_sequence<size_t, Rs...>, type_list<type_list<Hs...>>>
  {
  private:
    static constexpr size_t res_cnt = type_list_count_type<T, type_list<Hs...>>::value;
  public:
    using type = std::integer_sequence<size_t, Rs..., res_cnt>;
  };

  template <typename T, size_t... Rs, typename... Hs, typename... Ls >
  struct type_list_count_type_ll<T, std::integer_sequence<size_t, Rs...>, type_list<type_list<Hs...>, Ls...>>
  {
  private:
    static constexpr size_t res_cnt = type_list_count_type<T, type_list<Hs...>>::value;
  public:
    using type = typename type_list_count_type_ll<T, std::integer_sequence<size_t, Rs..., res_cnt>, type_list<Ls...>>::type;
  };

//-----------------------------------------------------------------------------
/// Wrap it; result is std::integral_constant<size_t, count> for plain list
/// For the list of lists it is std::integer_sequence<size_t, counts...>

  template <typename T, class List>
  struct type_list_deepcount_type;

  template <typename T, typename... Ls>
  struct type_list_deepcount_type<T, type_list<Ls...>>
  {
/// Condition to select path
  private:
    static constexpr bool bLL = is_listoflists<type_list<Ls...>>::value;
  public:
    using type = typename std::conditional<bLL,
      typename type_list_count_type_ll<T, std::integer_sequence<size_t>, type_list<Ls...>>::type,
      typename type_list_count_type<T, type_list<Ls...>>::type>::type;
  };

//-----------------------------------------------------------------------------
/// Get all indexes of a type in a TypeList; the result is a std::integer_sequence
/// Implementation

  template <size_t k, typename T, class List, class IntSeq>
  struct type_list_all_idx_impl;

  template <size_t k, typename T, size_t... Idx>                    // End of search
  struct type_list_all_idx_impl<k, T, type_list<>, std::integer_sequence<size_t, Idx...>>
  {
    using type = std::integer_sequence<size_t, Idx...>;
  };

  template <size_t k, typename T, typename... Ts, size_t... Idx>    // Type found at k
  struct type_list_all_idx_impl<k, T, type_list<T, Ts...>, std::integer_sequence<size_t, Idx...>>
  {
    using type =
      typename type_list_all_idx_impl<k + 1, T, type_list<Ts...>, std::integer_sequence<size_t, Idx..., k>>::type;
  };

  template <size_t k, typename T, typename H, typename... Ts, size_t... Idx> // Different type, continue
  struct type_list_all_idx_impl<k, T, type_list<H, Ts...>, std::integer_sequence<size_t, Idx...>>
  {
    using type =
      typename type_list_all_idx_impl<k + 1, T, type_list<Ts...>, std::integer_sequence<size_t, Idx...>>::type;
  };

/// Wrapper;; Supply the start index zero
  template <typename T, class List>
  struct type_list_all_idx;

  template <typename T, typename... Ts>
  struct type_list_all_idx<T, type_list<Ts...>>
  {
    using type = typename type_list_all_idx_impl<0, T, type_list<Ts...>, std::integer_sequence<size_t>>::type;
  };

//-----------------------------------------------------------------------------
/// Get all indexes of a type in each element of a list of lists
/// Do not use it as a standalone meta function; use type_list_all_deepidx instead

  template <typename T, class Res, class List>
  struct type_list_all_idx_ll
  {
    using type = type_list<std::integer_sequence<size_t>>;          // Will never called
  };

  template <typename T, typename... Rs, typename... Hs>         // End of search
  struct type_list_all_idx_ll<T, type_list<Rs...>, type_list<type_list<Hs...>>>
  {
  private:
    using iseq = typename type_list_all_idx_impl<0, T, type_list<Hs...>, std::integer_sequence<size_t>>::type;
  public:
    using type = type_list<Rs..., iseq>;
  };

  template <typename T, typename... Rs, typename... Hs, typename... ListEls>
  struct type_list_all_idx_ll<T, type_list<Rs...>, type_list<type_list<Hs...>, ListEls...>>
  {
  private:
    using iseq = typename type_list_all_idx_impl<0, T, type_list<Hs...>, std::integer_sequence<size_t>>::type;
  public:
    using type =
      typename type_list_all_idx_ll<T, type_list<Rs..., iseq>, type_list<ListEls...>>::type;
  };

//-----------------------------------------------------------------------------
/// Wrapper: find all indexes of the given type in a list or in elements of list of lists

  template <typename T, class List>
  struct type_list_all_deepidx;

  template <typename T, typename... ListEls>
  struct type_list_all_deepidx<T, type_list<ListEls...>>
  {
  private:
    static constexpr bool bLL = is_listoflists < type_list<ListEls...>>::value;
  public:
    using type = typename std::conditional < bLL,
      typename type_list_all_idx_ll<T, type_list<>, type_list<ListEls...>>::type,
      typename type_list_all_idx_impl<0, T, type_list<ListEls...>, std::integer_sequence<size_t>>::type>::type;
  };

//-----------------------------------------------------------------------------
/// Helper: Check whether all elements of a list are std::integer_sequences

  template <class List>
  struct is_listofiseqs                     // Called if the class is not a type_list
  {
    using type = std::integral_constant<bool, false>;
    using value_type = typename type::value_type;
    static constexpr value_type value = type::value;
  };

  template <>
  struct is_listofiseqs<type_list<>>
  {
    using type = std::integral_constant<bool, false>;
    using value_type = type::value_type;
    static constexpr value_type value = type::value;
  };

  template <typename H>
  struct is_listofiseqs<type_list<H>>
  {
  private:
    static constexpr bool bISeq = is_iseq<H>::value;
  public:
    using type = std::integral_constant<bool, bISeq>;
    using value_type = typename type::value_type;
    static constexpr value_type value = type::value;
  };
  
  template <typename H, typename... ISeqs>
  struct is_listofiseqs<type_list<H, ISeqs...>>
  {
  private:
    static constexpr bool bISeq = is_iseq<H>::value;
  public:
    using type = typename std::conditional < bISeq,
      typename is_listofiseqs<type_list<ISeqs...>>::type, std::integral_constant<bool, false>>::type;
    using value_type = typename type::value_type;
    static constexpr value_type value = type::value;
  };

//-----------------------------------------------------------------------------
/// Reverse order of members of the list implementation: needs empty list as destiny

  template <class Src, class Rsvr>
  struct type_list_reverse_impl;

  template <typename... Rs>
  struct type_list_reverse_impl<type_list<>, type_list<Rs...>>
  {
    using type = type_list<Rs...>;
  };

  template <class H, class... Rs >
  struct type_list_reverse_impl<type_list<H>, type_list<Rs...>>
  {
    using type = type_list <H, Rs... >;
  };

  template <class H, class... Ts, class... Rs>
  struct type_list_reverse_impl<type_list<H, Ts...>, type_list<Rs...>>
  {
    using type = typename type_list_reverse_impl <  type_list<Ts...>, type_list<H, Rs...>>::type;
  };

/// Wrapper to hide starting empty resulting list inside the wrapper

  template <class List>
  struct type_list_reverse; 

  template <typename... Ts>
  struct type_list_reverse<type_list<Ts...>>
  {
    using type = typename type_list_reverse_impl<type_list<Ts...>, type_list<>>::type;
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

//-----------------------------------------------------------------------------
/// Helper: Move first k elements from list L2 to the tail of the list T1
/// Type is the list of two lists, one with added elements, other with a rest

  template <size_t k, class Lst1, class Lst2>
  struct type_list_move_firsts;
  
  template <typename H, typename... T1s, typename... T2s>
  struct type_list_move_firsts<1, type_list<T1s...>, type_list<H, T2s...>>
  {
    using type = type_list < type_list<T1s..., H>, type_list<T2s... >>;
  };

  template <size_t k, typename H, typename... T1s, typename... T2s>
  struct type_list_move_firsts<k, type_list<T1s...>, type_list<H, T2s...>>
  {
    static_assert (k > 0, "Number of elements to transfer must be > 0");
    using type = typename type_list_move_firsts<k - 1, type_list<T1s..., H>, type_list<T2s...>>::type;
  };

//---------------------------------------------------------------------------
/// Split a TypeList into multiple lists; in type_list_split_impl numbers of elements 
//  in lists are in integer sequence

  template <class Idx, class List1, class List2>
  struct type_list_split_impl;

  template <size_t l, typename... Lists, typename... Tails>
  struct type_list_split_impl<std::integer_sequence<size_t, l>, type_list<Lists...>, type_list<Tails...>>
  {
  private:
    using lst_split_2 = typename type_list_move_firsts<l, type_list<>, type_list<Tails...>>::type;
    using first = typename type_list_type_at<0, lst_split_2>::type;
    using second = typename type_list_type_at<1, lst_split_2>::type;
  public:
    using type = type_list<Lists..., first, second>;
  };

  template <size_t l, size_t... Ls, typename ... Lists, typename... Tails>
  struct type_list_split_impl < std::integer_sequence<size_t, l, Ls...>, type_list<Lists...>, type_list<Tails...>>
  {
  private:
    using lst_split_2 = typename type_list_move_firsts<l, type_list<>, type_list<Tails...>>::type;
    using first = typename type_list_type_at<0, lst_split_2>::type;
    using second = typename type_list_type_at<1, lst_split_2>::type;
    using lst_res = type_list<Lists..., first>;
  public:
    using type = typename type_list_split_impl<std::integer_sequence<size_t, Ls...>, lst_res, second>::type;
  };

/// Wrapper to save the user from generating std::integer_sequence; will do it inside the structure

  template < class List, size_t... Ls>
  struct type_list_split;

  template <typename... Ts, size_t... Ls>
  struct type_list_split<type_list<Ts...>, Ls...>
  {
  private:
// Check the sum of component lengths; must be less than size of original list
    using lengths = std::integer_sequence<size_t, Ls...>;
    using lengths_sum = typename iseq_sum<lengths>::type;
    static_assert(lengths_sum::value < type_list<Ts...>::size(), 
      "type_list_split: Length of a last component must be greater than zero; check the sum of component sizes");
// No zero lengths are allowed
    static_assert(iseq_index_of<size_t, 0, lengths>::value == -1,
      "Multi_Split: Remove components with zero size from the sequence of sizes");
  public:   // Go
    using type = typename type_list_split_impl<std::integer_sequence<size_t, Ls...>, type_list<>, type_list<Ts...>>::type;
  };

//-----------------------------------------------------------------------------
/// type_list_max_type_impl gets first element of max size in a plain list

  template <typename T, class List>
  struct type_list_max_type_impl; 

  template <typename T>                   // End of recursion
  struct type_list_max_type_impl<T, type_list<>>
  {
    using max_type = T;
  };

  template <typename T, typename H, typename... Ts>
  struct type_list_max_type_impl<T, type_list<H, Ts...>>
  {
  private:
    static constexpr bool  bRes = Sizeof<T>::value >= Sizeof<H>::value ? true : false;
    using selected = typename std::conditional<bRes, T, H>::type;
  public:
    using max_type = typename type_list_max_type_impl<selected, type_list<Ts...>>::max_type;
  };

/// Wrapper to apply to a plain list (Starts from a NullType, size = 0
  template <class List>
  struct type_list_max_type_size; 

  template <typename... Ts>
  struct type_list_max_type_size<type_list<Ts...>>
  {
  private:
    using max_type = typename type_list_max_type_impl < NullType, type_list<Ts... >> ::max_type;
  public:
    using type = std::integral_constant<size_t, Sizeof<max_type>::value>;
    using value_type = typename type::value_type;
    static constexpr value_type value = type::value;
  };

//-----------------------------------------------------------------------------
/// Find max size of elements in each list of a list of lists

  template <class ISeq, class List>
  struct type_list_max_type_size_ll    // Never executed, but needed for std::std::conditional
  {
    using type = std::integer_sequence<size_t>;
  };

  template <size_t ... Ids, typename... Hs>             // End of recursion
  struct type_list_max_type_size_ll<std::integer_sequence<size_t, Ids...>, type_list<type_list<Hs...>>>
  {
  private:
    static constexpr size_t loc_max = type_list_max_type_size<type_list<Hs...>>::value;
  public:
    using type = std::integer_sequence<size_t, Ids..., loc_max>;
  };

  template <size_t... Ids, typename... Hs, typename... ListEls>
  struct type_list_max_type_size_ll<std::integer_sequence<size_t, Ids...>, type_list<type_list<Hs...>, ListEls...>>
  {
  private:
    static constexpr size_t loc_max = type_list_max_type_size<type_list<Hs...>>::value;
  public:
    using type =
      typename type_list_max_type_size_ll<std::integer_sequence<size_t, Ids..., loc_max>, type_list<ListEls...>>::type;
  };

//-----------------------------------------------------------------------------
/// Wrapper: find max size of elements in a plain list or max elements in
///a list of lists

  template <class List>
  struct type_list_max_type_deepsize;

  template <typename... ListEls>
  struct type_list_max_type_deepsize<type_list<ListEls...>>
  {
  private:
    static constexpr bool bLL = is_listoflists<type_list<ListEls...>>::value;
  public:
    using type = typename std::conditional < bLL,
      typename type_list_max_type_size_ll < std::integer_sequence<size_t>, type_list<ListEls...>>::type,
      typename type_list_max_type_size<type_list<ListEls...>>::type > ::type;
 };

//-----------------------------------------------------------------------------
/// Find the first element of given size in the list of mixed element types

  template <size_t k, class List>
  struct type_list_firsttype_of_size;

  template <size_t k>
  struct type_list_firsttype_of_size<k, type_list<>>
  {
    using type = std::false_type;
  };

  template <size_t k, typename H, typename... Ts>
  struct type_list_firsttype_of_size< k, type_list<H, Ts...>>
  {
    using type =
      typename std::conditional < Sizeof<H>::value == k, H,
      typename type_list_firsttype_of_size<k, type_list<Ts...>>::type>::type;
  };

//------------------------------------------------------------------------------
/// Find the first element of given size in each list of a list of lists; result is a type_list
 
    template <size_t k, class ResLst, class List> /// It is only to compile; 
    struct type_list_firstltype_of_size                /// a branch in the wrapper
    {                                             /// is never called
      using type = type_list<>;
    };
   

    template <size_t k, typename... Rs, typename... Hs>           // Stop recursion
    struct type_list_firstltype_of_size<k, type_list<Rs...>, type_list<type_list<Hs...>>>
    {
    private:
      using type_found = typename type_list_firsttype_of_size<k, type_list<Hs...>>::type;
    public:
      using type = type_list<Rs..., type_found>;
    };

    template <size_t k, typename... Rs, typename... Hs, typename... Ts>
    struct type_list_firstltype_of_size < k, type_list <Rs...>, type_list<type_list<Hs...>, Ts...>>
    {
    private:
      using type_found = typename type_list_firsttype_of_size < k, type_list<Hs...>>::type;
    public:
      using type = typename type_list_firstltype_of_size<k, type_list<Rs..., type_found>, type_list<Ts...>>::type;
    };

//---------------------------------------------------------------------------
/// Wrapper

  template <size_t k, class List>
  struct type_list_first_typedeep_of_size;
  
   template <size_t k, typename... ListEls>
   struct type_list_first_typedeep_of_size<k, type_list<ListEls...>>
   {
   private:
     static constexpr bool bLs = is_listoflists<type_list<ListEls...>>::value;
   public:
     using type = typename std::conditional < bLs, 
       typename type_list_firstltype_of_size < k, type_list<>, type_list<ListEls...>>::type,
       typename type_list_firsttype_of_size<k, type_list<ListEls...>>::type>::type;
   };
  
//-----------------------------------------------------------------------------
/// Get all types of given size from the plain list of mixed elements, no duplicates

  template <size_t k, class ListRes, class ListSoource>
  struct type_list_all_ttypes_of_size;

  template <size_t k, typename... Rs>
  struct type_list_all_ttypes_of_size<k, type_list<Rs...>, type_list<>>
  {
   using type = typename type_list_erase_dupl<type_list<Rs...>>::type;
  };

  template <size_t k, typename H, typename... Rs, typename... Ts>
  struct type_list_all_ttypes_of_size<k, type_list<Rs...>, type_list<H, Ts...>>
  {
  private:
    using res_lst = typename std::conditional < Sizeof<H>::value == k, type_list<Rs..., H>, type_list < Rs...>>::type;
  public:
    using type =
      typename type_list_all_ttypes_of_size<k, res_lst, type_list<Ts...>>::type;
  };

//---------------------------------------------------------------------------
/// Wrapper to use without looking inside the list's elements of type_list type 

  template <size_t k, class List>
  struct type_list_all_types_of_size;

  template <size_t k, typename... Ts>
  struct type_list_all_types_of_size<k, type_list<Ts...>>
  {
    using type = 
      typename type_list_all_ttypes_of_size<k, type_list<>, type_list<Ts...>>::type;
  };
//---------------------------------------------------------------------------
/// Get all types of size k from a list of lists

  template <size_t k, class ListRes, class ListSource>
  struct type_list_all_ltypes_of_size
  {
    using type = type_list<>;
  };

  template <size_t k, typename... RLs, typename... Hs>
  struct type_list_all_ltypes_of_size<k, type_list<RLs...>, type_list<type_list<Hs...>>>
  {
  private:
    using types_found = typename type_list_all_types_of_size < k, type_list<Hs...>>::type;
  public:
    using type = type_list<RLs..., types_found>;
  };

  template <size_t k, typename... RLs, typename... Hs, typename... Ts> 
  struct type_list_all_ltypes_of_size<k, type_list<RLs...>, type_list<type_list<Hs...>, Ts...>>
  {
  private:
    using types_found = typename type_list_all_types_of_size<k, type_list<Hs...>>::type;
  public:
    using type = 
      typename type_list_all_ltypes_of_size < k, type_list<RLs..., types_found>, type_list<Ts...>>::type;
  };

//---------------------------------------------------------------------------
/// Wrapper

  template <size_t k, class List>
  struct type_list_all_typesdeep_of_size;

  template <size_t k, typename... ListEls>
  struct type_list_all_typesdeep_of_size<k, type_list<ListEls...>>
  {
  private:
    static constexpr bool bLs = is_listoflists<type_list<ListEls...>>::value;
  public:
    using type = typename std::conditional < bLs, 
      typename type_list_all_ltypes_of_size < k, type_list<>, type_list<ListEls...>>::type,
      typename type_list_all_ttypes_of_size<k, type_list<>, type_list<ListEls...>>::type>::type;
  };

//-----------------------------------------------------------------------------
/// Get all types of greater or greater and equal (bEqual ==true) of given size 
///  from the list of mixed elements, no duplicates

    template <bool bEqual, size_t k, class ListRes, class ListSoource>
    struct type_list_all_ttypes_greater;

    template <bool bEqual, size_t k, typename... Rs>
    struct type_list_all_ttypes_greater<bEqual, k, type_list<Rs...>, type_list<>>
    {
      using type = typename type_list_erase_dupl<type_list<Rs...>>::type;
    };

    template <bool bEqual, size_t k, typename H, typename... Rs, typename... Ts>
    struct type_list_all_ttypes_greater<bEqual, k, type_list<Rs...>, type_list<H, Ts...>>
    {
    private:
      static constexpr bool bComp = bEqual ? (Sizeof<H>::value) >= k : (Sizeof<H>::value) > k;
      using res_lst =
        typename std::conditional < bComp, type_list<Rs..., H>, type_list < Rs...>>::type;
    public:
      using type =
        typename type_list_all_ttypes_greater<bEqual, k, res_lst, type_list<Ts...>>::type;
    };

/// Wrapper to provide initial empty list for result
    template <bool bEqual, size_t k, class List>
    struct type_list_all_types_greater;

    template <bool bEqual, size_t k, typename... Ts>
    struct type_list_all_types_greater<bEqual, k, type_list<Ts...>>
    {
      using type = typename type_list_all_ttypes_greater<bEqual, k, type_list<>, type_list<Ts...>>::type;
    };
 
//-----------------------------------------------------------------------------
/// All types greater or equal of size k in a list of lists

  template <bool bEqual, size_t k, class ListRes, class ListSource>
  struct type_list_all_ltypes_greater
  {
    using type = type_list<>;
  };

  template <bool bEqual, size_t k, typename... RLs, typename... Hs>
  struct type_list_all_ltypes_greater<bEqual, k, type_list<RLs...>, type_list<type_list<Hs...>>>
  {
  private:
    using types_found = typename type_list_all_types_greater<bEqual, k, type_list<Hs...>>::type;
  public:
    using type = type_list<RLs..., types_found>; ;
  };

  template <bool bEqual, size_t k, typename... RLs, typename... Hs, typename... Ts>
  struct type_list_all_ltypes_greater<bEqual, k, type_list<RLs...>, type_list<type_list<Hs...>, Ts...>>
  {
  private:
    using types_found = typename type_list_all_types_greater<bEqual, k, type_list<Hs...>>::type;
  public:
    using type =
      typename type_list_all_ltypes_greater <bEqual, k, type_list<RLs..., types_found>, type_list<Ts...>>::type;
  };

//-----------------------------------------------------------------------------
/// Wrapper for types greater than size k

  template <bool bEqual, size_t k, class ListSources>
  struct type_list_all_typesdeep_greater;

  template <bool bEqual, size_t k, typename... ListEls>
  struct type_list_all_typesdeep_greater<bEqual, k, type_list<ListEls...>>
  {
  private:
    static constexpr bool bLs = is_listoflists<type_list<ListEls...>>::value;
  public:
    using type = typename std::conditional < bLs,
      typename type_list_all_ltypes_greater <bEqual, k, type_list<>, type_list<ListEls...>>::type,
      typename type_list_all_ttypes_greater<bEqual, k, type_list<>, type_list<ListEls...>>::type>::type;
  };

//-----------------------------------------------------------------------------
/// All types less or equal the given size (supply true to include equal)
/// for a list of mixed elements

  template <bool bEqual, size_t k, class ListRes, class ListSoource>
  struct type_list_all_ttypes_less;

  template <bool bEqual, size_t k, typename... Rs>
  struct type_list_all_ttypes_less<bEqual, k, type_list<Rs...>, type_list<>>
  {
    using type = typename type_list_erase_dupl<type_list<Rs...>>::type;
  };

  template <bool bEqual, size_t k, typename H, typename... Rs, typename... Ts>
  struct type_list_all_ttypes_less<bEqual, k, type_list<Rs...>, type_list<H, Ts...>>
  {
  private:
    static constexpr bool bComp = bEqual ? (Sizeof<H>::value) <= k : (Sizeof<H>::value) < k;
    using res_lst =
      typename std::conditional < bComp, type_list<Rs..., H>, type_list < Rs...>>::type;
  public:
    using type =
      typename type_list_all_ttypes_less<bEqual, k, res_lst, type_list<Ts...>>::type;
  };

// Initial result list wrapped
  template <bool bEqual, size_t k, class List>
  struct type_list_all_types_less;

  template <bool bEqual, size_t k, typename... Ts>
  struct type_list_all_types_less<bEqual, k, type_list<Ts...>>
  {
    using type = typename type_list_all_ttypes_less<bEqual, k, type_list<>, type_list<Ts...>>::type;
  };

//-----------------------------------------------------------------------------
/// All types less or equal size k in a list of lists

  template <bool bEqual, size_t k, class ListRes, class ListSource>
  struct type_list_all_ltypes_less
  {
    using type = type_list<>;
  };

  template <bool bEqual, size_t k, typename... RLs, typename... Hs>
  struct type_list_all_ltypes_less<bEqual, k, type_list<RLs...>, type_list<type_list<Hs...>>>
  {
  private:
    using types_found = typename type_list_all_types_less<bEqual, k, type_list<Hs...>>::type;  
  public:
    using type = type_list<RLs..., types_found>; ;
  };

  template <bool bEqual, size_t k, typename... RLs, typename... Ts, typename... Hs>
  struct type_list_all_ltypes_less<bEqual, k, type_list<RLs...>, type_list<type_list<Hs...>, Ts...>>
  {
  private:
    using types_found = typename type_list_all_ttypes_less<bEqual, k, type_list<>, type_list<Hs...>>::type;
  public:
    using type =
      typename type_list_all_ltypes_less <bEqual, k, type_list<RLs..., types_found>, type_list<Ts...>>::type;
  };

//-----------------------------------------------------------------------------
/// Wrapper for types lesser or equal than size k

  template <bool bEqual, size_t k, class ListSources>
  struct type_list_all_typesdeep_less;

  template <bool bEqual, size_t k, typename... ListEls>
  struct type_list_all_typesdeep_less<bEqual, k, type_list<ListEls...>>
  {
  private:
    static constexpr bool bLs = is_listoflists<type_list<ListEls...>>::value;
  public:
    using type = typename std::conditional < bLs,
      typename type_list_all_ltypes_less <bEqual, k, type_list<>, type_list<ListEls...>>::type,
      typename type_list_all_ttypes_less<bEqual, k, type_list<>, type_list<ListEls...>>::type>::type;
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

}// End of namespace TypeList

