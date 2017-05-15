#pragma once

#include <tuple>
#include <assert.h>

#include "TypeList.h"
#include "TupleManipulation.h"

#define PANKU_LIST(...) \
   using PankuUserProvidedList = type_list<__VA_ARGS__>; \
   using PankuProcessedList = PankuMetaprogram::type_list_extract_heads<PankuUserProvidedList>; \
   using PankuUserClassList = PankuProcessedList::type; \
   using PankuDependencies = PankuProcessedList::headless_type; \
   using PankuPath = PankuMetaprogram::topological_sort<PankuUserClassList, PankuDependencies>::type; \
   using PankuPathPointerised = PankuMetaprogram::type_list_pointerise<PankuPath>::type; \
   using PankuClassTuple = convert<PankuPathPointerised, std::tuple>; \
   extern Panku<PankuClassTuple> panku;

#define PANKU_INSTANCE Panku<PankuClassTuple> panku;

#define STANDALONE(ClassName) type_list<ClassName>
#define DEPENDENCY(ClassName, ...) type_list<ClassName, ##__VA_ARGS__>

using namespace TypeList;

template<typename UserClass>
UserClass& ConstructAndInitialise();

template<typename Tuple>
class Panku
{
   public:
   Panku() 
   {
      TupleManipulation::for_each_in_tuple(userClassTuple, [](auto& element)
            { element = &ConstructAndInitialise<decltype(*element)>(); });
   }

   template<typename UserClass>
   UserClass& Get() 
   {
      auto userObjectPointer = std::get<UserClass*>(userClassTuple);
      assert(userObjectPointer);
      return *userObjectPointer;
   }

   private:
   Tuple userClassTuple;
};

namespace PankuMetaprogram
{

   // Turns a type list into a type list of its pointer types.
   template<class List>
   struct type_list_pointerise;

   template<>
   struct type_list_pointerise<type_list<>>
   {
      using type = type_list<>;
   };

   template<typename T, typename... Ts>
   struct type_list_pointerise<type_list<T, Ts...>>
   {
      using type = typename type_list_push_front<T*, typename type_list_pointerise<type_list<Ts...>>::type>::type;
   };

   // Extracts the first type from each list in a list of lists
   template <class ListOfLists>
   struct type_list_extract_heads;

   template<>
   struct type_list_extract_heads<type_list<>>
   {
      using type = type_list<>;
      using headless_type = type_list<>;
   };

   template<typename T, typename... OtherLists>
   struct type_list_extract_heads<type_list<type_list<T>, OtherLists...>>
   {
      using type = typename type_list_push_front<T, typename type_list_extract_heads<type_list<OtherLists...>>::type>::type;
      using headless_type = typename type_list_push_front<type_list<>, typename type_list_extract_heads<type_list<OtherLists...>>::headless_type>::type;
   };

   template<typename T, typename... Ts, typename... OtherLists>
   struct type_list_extract_heads<type_list<type_list<T, Ts...>, OtherLists...>>
   {
      using type = typename type_list_push_front<T, typename type_list_extract_heads<type_list<OtherLists...>>::type>::type;
      using headless_type = typename type_list_push_front<type_list<Ts...>, typename type_list_extract_heads<type_list<OtherLists...>>::headless_type>::type;
   };

   ////////////////////////////////////////////////
   // Check all elements of list A are in list B //
   ////////////////////////////////////////////////
   template<class ListA, class ListB>
   struct check_all_in;

   // By default, all elements in an empty list are in another list
   template<typename ListB> 
   struct check_all_in<type_list<>, ListB>
   {
      using type = std::true_type; 
      using value_type = type::value_type;
      static constexpr value_type value = type::value;
   };

   // Recursive case
   template<typename T, typename... Ts, typename ListB>
   struct check_all_in<type_list<T, Ts...>, ListB>
   {
   private:
      static constexpr bool TInListB = type_list_index_of<T, ListB>::value != -1;
   public:
      using type = typename std::conditional < TInListB, 
                                               typename check_all_in<type_list<Ts...>, ListB>::type,
                                               std::integral_constant<bool, false >>::type;
      using value_type = typename type::value_type;
      static constexpr value_type value = type::value;
   };

   // Compile time topological sort 
   //////////////////////////////////
   // Attempts to construct a path through each of the user classes, so that each is
   // initialised after all of its dependencies.
   //////////////////////////////////

   // Constructs list of nodes that are not yet in the path, but whose dependencies
   // are in the path
   template<class UserClassList, class UserClassDependencies, class Path>
   struct extract_candidates;

   template<typename ListB, typename Path>
   struct extract_candidates<type_list<>, ListB, Path>
   {
      using type = type_list<>; 
   };

   template<typename T, typename Path, typename... Ts, typename D, typename... Ds>
   struct extract_candidates<type_list<T, Ts...>, type_list<D, Ds...>, Path>
   {
   private:
      static constexpr bool UserClassInPath = type_list_index_of<T,Path>::value != -1;
      static constexpr bool AllUserClassDependenciesInPath = check_all_in<D, Path>::value;
      static constexpr bool UserClassIsCandidate = !UserClassInPath && AllUserClassDependenciesInPath;
      using next_recursive_step = typename extract_candidates<type_list<Ts...>, type_list<Ds...>, Path>::type;
   public:

      // If all conditions hold, our candidates list consist of the front element T plus 
      // the recursive application of the algorithm in the remainder of the list. Otherwise
      // T Is skipped
      using type = typename std::conditional < UserClassIsCandidate,
                      typename type_list_push_front<T, next_recursive_step>::type,
                      next_recursive_step
                   >::type;
                                             
   };

   template <typename UserClassList, typename UserClassDependencies, typename Path, typename Specialization = void>
   struct topological_sort_step
   {
   private:
      using Candidates = typename extract_candidates<UserClassList, UserClassDependencies, Path>::type;
      using NewPath = typename type_list_concat_lists<Path, Candidates>::type;
      static constexpr bool Finished = UserClassList::size() == NewPath::size();
      static constexpr bool Stuck = Candidates::size() == 0;
      static_assert(Finished || !Stuck, "Panku dependency graph is not solvable."
                                        " Please beware of circular dependencies");

   public:
      using type = typename topological_sort_step<UserClassList, UserClassDependencies, NewPath>::type;

   };

   template<typename UserClassList, typename UserClassDependencies, typename Path>
   struct topological_sort_step<UserClassList, UserClassDependencies, Path, typename std::enable_if<UserClassList::size() == Path::size()>::type>
   {
      using type = typename Path::type;
   };

   // Performs a recursive depth-first search to sort the DAG topologically
   template <typename UserClassList, typename UserClassDependencies>
   struct topological_sort
   {
   public:
      using type = typename topological_sort_step<UserClassList, UserClassDependencies, type_list<>>::type;
   };

namespace test_picking_heads_from_list_of_lists
{
   // Given
   using ListOfLists = type_list<type_list<float, double>, type_list<int, long>>;

   // When
   using HeadList = type_list_extract_heads<ListOfLists>::type;
   
   // Then
   constexpr auto expectedSize = 2;

   static_assert(expectedSize == HeadList::size());
   static_assert(std::is_same<float, type_list_type_at<0, HeadList>::type >::value);
   static_assert(std::is_same<int, type_list_type_at<1, HeadList>::type >::value);
}

namespace test_check_elements_from_one_list_exist_in_another 
{
   // Given
   using SmallList = type_list<char, double>;
   using BigList = type_list<char, int, double>;

   // Then
   static_assert(check_all_in<SmallList, BigList>::value, "check_all_in fails for list subsets.");

   // And the reverse...
   static_assert(!check_all_in<BigList, SmallList>::value, "check_all_in returns true for list supersets.");
}

namespace extract_candidates_for_nodes_with_no_dependencies
{
   // Given
   using SampleTypes = type_list<char, double, int>;
   using Dependencies = type_list<type_list<>, type_list<>, type_list<>>;
   using Path = type_list<>;
   constexpr auto expectedSize = 3;

   // When
   using Candidates = extract_candidates<SampleTypes, Dependencies, Path>::type;

   // Then
   static_assert(expectedSize == Candidates::size());
   static_assert(std::is_same<char, type_list_type_at<0, Candidates>::type >::value);
   static_assert(std::is_same<double, type_list_type_at<1, Candidates>::type >::value);
   static_assert(std::is_same<int, type_list_type_at<2, Candidates>::type >::value);
}

namespace extract_candidates_for_nodes_with_dependencies_and_empty_path
{
   // Given
   using SampleTypes = type_list<long, char, double, float, int>;
   // Arbitrarily, let's say that double depends on float, int depends on char,
   // and long depends on int and char
   using Dependencies = type_list<type_list<int,char>, type_list<>, type_list<float>, type_list<>, type_list<char>>;  
   using Path = type_list<>;
   constexpr auto expectedSize = 2;

   // When
   using Candidates = extract_candidates<SampleTypes, Dependencies, Path>::type;

   // Then
   static_assert(expectedSize == Candidates::size());
   static_assert(std::is_same<char, type_list_type_at<0, Candidates>::type >::value);
   static_assert(std::is_same<float, type_list_type_at<1, Candidates>::type >::value);
}

namespace extract_candidates_for_nodes_without_dependencies_and_partially_full_path
{
   // Given
   using SampleTypes = type_list<long, char, double, float, int>;
   using Dependencies = type_list<type_list<>, type_list<>, type_list<>, type_list<>, type_list<>>;  
   using Path = type_list<double, int>;
   constexpr auto expectedSize = 3;

   // When
   using Candidates = extract_candidates<SampleTypes, Dependencies, Path>::type;

   // Then
   static_assert(expectedSize == Candidates::size());
   static_assert(std::is_same<long, type_list_type_at<0, Candidates>::type >::value);
   static_assert(std::is_same<char, type_list_type_at<1, Candidates>::type >::value);
   static_assert(std::is_same<float, type_list_type_at<2, Candidates>::type >::value);
}

namespace sequential_extraction_of_candidates
{
   // Given
   // We repeat the extraction steps with an empty path and some dependencies
   using SampleTypes = type_list<long, char, double, float, int>;
   using Dependencies = type_list<type_list<int,char>, type_list<>, type_list<float>, type_list<>, type_list<char>>;  
   using Path = type_list<>;
   using FirstPassCandidates = extract_candidates<SampleTypes, Dependencies, Path>::type;

   // When
   // We move the candidates from the first pass to the path (so it becomes the new Path)
   using SecondPassPath = FirstPassCandidates;
   using SecondPassCandidates = extract_candidates<SampleTypes, Dependencies, SecondPassPath>::type;
   // Since we already have char and float on the path, int and double can be added to
   // the path, but not yet long, because it depends on int
   constexpr auto expectedSizeSecondPass = 2;

   // Then
   static_assert(expectedSizeSecondPass == SecondPassCandidates::size());
   static_assert(std::is_same<double, type_list_type_at<0, SecondPassCandidates>::type >::value);
   static_assert(std::is_same<int, type_list_type_at<1, SecondPassCandidates>::type >::value);

   // When
   using ThirdPassPath = type_list_concat_lists<FirstPassCandidates, SecondPassCandidates>::type;
   using ThirdPassCandidates = extract_candidates<SampleTypes, Dependencies, ThirdPassPath>::type;
   constexpr auto expectedSizeThirdPass = 1;
   static_assert(expectedSizeThirdPass == ThirdPassCandidates::size());
   static_assert(std::is_same<long, type_list_type_at<0, ThirdPassCandidates>::type >::value);
}

namespace full_topological_sort
{
   // Given
   using SampleTypes = type_list<long, char, double, float, int>;
   // Arbitrarily, let's say that double depends on float, int depends on char,
   // and long depends on int and char
   using Dependencies = type_list<type_list<int,char>, type_list<>, type_list<float>, type_list<>, type_list<char>>;  

   // When
   using SortedList = topological_sort<SampleTypes, Dependencies>::type;

   // Then the list is sorted in dependency order
   constexpr auto expectedSize = 5;
   static_assert(expectedSize == SortedList::size());
   static_assert(std::is_same<char, type_list_type_at<0, SortedList>::type >::value);
   static_assert(std::is_same<float, type_list_type_at<1, SortedList>::type >::value);
   static_assert(std::is_same<double, type_list_type_at<2, SortedList>::type >::value);
   static_assert(std::is_same<int, type_list_type_at<3, SortedList>::type >::value);
   static_assert(std::is_same<long, type_list_type_at<4, SortedList>::type >::value);
}

namespace another_full_topological_sort
{
   // Given
   class Alpha;
   class Beta;
   class Gamma;

   using SampleTypes = type_list<Beta, Alpha, Gamma>;
   // Arbitrarily, let's say that double depends on float, int depends on char,
   // and long depends on int and char
   using Dependencies = type_list<type_list<Alpha>, type_list<>, type_list<Alpha, Beta>>;

   // When
   using SortedList = topological_sort<SampleTypes, Dependencies>::type;

   // Then the list is sorted in dependency order
   constexpr auto expectedSize = 3;
   static_assert(expectedSize == SortedList::size());
   static_assert(std::is_same<Alpha, type_list_type_at<0, SortedList>::type >::value);
   static_assert(std::is_same<Beta, type_list_type_at<1, SortedList>::type >::value);
   static_assert(std::is_same<Gamma, type_list_type_at<2, SortedList>::type >::value);
}

}
