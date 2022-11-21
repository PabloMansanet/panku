#pragma once

namespace NamedPankuMetaprogram
{

namespace test_picking_heads_from_list_of_lists
{
   // Given
   using ListOfLists = TypeList::type_list<TypeList::type_list<float, double>,
                                           TypeList::type_list<int, long>>;

   // When
   using HeadList = type_list_extract_heads<ListOfLists>::type;

   // Then
   constexpr auto expectedSize = 2;

   static_assert(expectedSize == HeadList::size());
   static_assert(std::is_same<float, TypeList::type_list_type_at<0, HeadList>::type >::value);
   static_assert(std::is_same<int, TypeList::type_list_type_at<1, HeadList>::type >::value);
}

namespace test_check_elements_from_one_list_exist_in_another
{
   // Given
   using SmallList = TypeList::type_list<char, double>;
   using BigList = TypeList::type_list<char, int, double>;

   // Then
   static_assert(check_all_in<SmallList, BigList>::value, "check_all_in fails for list subsets.");

   // And the reverse...
   static_assert(!check_all_in<BigList, SmallList>::value, "check_all_in returns true for list supersets.");
}

namespace test_collection_types_work_as_dependencies
{
   // Given
   using DependencyList = TypeList::type_list<char>;
   using AlreadyInitialised = TypeList::type_list<NamedPankuMetaprogram::Collection<char,2>>;

   // Then
   static_assert(check_all_in<DependencyList, AlreadyInitialised>::value, "Collection types are not counted as dependencies.");
}

namespace extract_candidates_for_nodes_with_no_dependencies
{
   // Given
   using SampleTypes = TypeList::type_list<char, double, int>;
   using Dependencies = TypeList::type_list<TypeList::type_list<>, TypeList::type_list<>, TypeList::type_list<>>;
   using Path = TypeList::type_list<>;
   constexpr auto expectedSize = 3;

   // When
   using Candidates = extract_candidates<SampleTypes, Dependencies, Path>::type;

   // Then
   static_assert(expectedSize == Candidates::size());
   static_assert(std::is_same<char, TypeList::type_list_type_at<0, Candidates>::type >::value);
   static_assert(std::is_same<double, TypeList::type_list_type_at<1, Candidates>::type >::value);
   static_assert(std::is_same<int, TypeList::type_list_type_at<2, Candidates>::type >::value);
}

namespace extract_candidates_for_nodes_with_dependencies_and_empty_path
{
   // Given
   class Trousers;  // Must be worn after underwear.
   class Underwear;
   class Shirt;
   class Shoes;  // Must be worn after socks and trousers.
   class Socks;
   class Belt;   // Must be worn after trousers and shirt.
   class Jacket; // Must be worn after shirt.

   using SampleTypes = TypeList::type_list<Trousers, Underwear, Shirt, Shoes, Socks, Belt, Jacket>;
   using Dependencies = TypeList::type_list<
                           TypeList::type_list<Underwear>,
                           TypeList::type_list<>,
                           TypeList::type_list<>,
                           TypeList::type_list<Socks, Trousers>,
                           TypeList::type_list<>,
                           TypeList::type_list<Trousers, Shirt>,
                           TypeList::type_list<Shirt>
                        >;

   using Path = TypeList::type_list<>;
   constexpr auto expectedSize = 3;

   // When
   using Candidates = extract_candidates<SampleTypes, Dependencies, Path>::type;

   // Then
   static_assert(expectedSize == Candidates::size());
   static_assert(std::is_same<Underwear, TypeList::type_list_type_at<0, Candidates>::type >::value);
   static_assert(std::is_same<Shirt, TypeList::type_list_type_at<1, Candidates>::type >::value);
   static_assert(std::is_same<Socks, TypeList::type_list_type_at<2, Candidates>::type >::value);
}

namespace extract_candidates_for_nodes_without_dependencies_and_partially_full_path
{
   // Given
   using SampleTypes = TypeList::type_list<long, char, double, float, int>;
   using Dependencies = TypeList::type_list<TypeList::type_list<>,
                                            TypeList::type_list<>,
                                            TypeList::type_list<>,
                                            TypeList::type_list<>,
                                            TypeList::type_list<>>;
   using Path = TypeList::type_list<double, int>;
   constexpr auto expectedSize = 3;

   // When
   using Candidates = extract_candidates<SampleTypes, Dependencies, Path>::type;

   // Then
   static_assert(expectedSize == Candidates::size());
   static_assert(std::is_same<long, TypeList::type_list_type_at<0, Candidates>::type >::value);
   static_assert(std::is_same<char, TypeList::type_list_type_at<1, Candidates>::type >::value);
   static_assert(std::is_same<float, TypeList::type_list_type_at<2, Candidates>::type >::value);
}

namespace sequential_extraction_of_candidates
{
   // Given
   class Trousers;  // Must be worn after underwear.
   class Underwear;
   class Shirt;
   class Shoes;  // Must be worn after socks and trousers.
   class Socks;
   class Belt;   // Must be worn after trousers and shirt.
   class Jacket; // Must be worn after shirt.

   using SampleTypes = TypeList::type_list<Trousers, Underwear, Shirt, Shoes, Socks, Belt, Jacket>;
   using Dependencies = TypeList::type_list<
                           TypeList::type_list<Underwear>,
                           TypeList::type_list<>,
                           TypeList::type_list<>,
                           TypeList::type_list<Socks, Trousers>,
                           TypeList::type_list<>,
                           TypeList::type_list<Trousers, Shirt>,
                           TypeList::type_list<Shirt>
                        >;

   using Path = TypeList::type_list<>;
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
   static_assert(std::is_same<Trousers, TypeList::type_list_type_at<0, SecondPassCandidates>::type >::value);
   static_assert(std::is_same<Jacket, TypeList::type_list_type_at<1, SecondPassCandidates>::type >::value);

   // When
   using ThirdPassPath = TypeList::type_list_concat_lists<FirstPassCandidates, SecondPassCandidates>::type;
   using ThirdPassCandidates = extract_candidates<SampleTypes, Dependencies, ThirdPassPath>::type;
   constexpr auto expectedSizeThirdPass = 2;
   static_assert(expectedSizeThirdPass == ThirdPassCandidates::size());
   static_assert(std::is_same<Shoes, TypeList::type_list_type_at<0, ThirdPassCandidates>::type >::value);
   static_assert(std::is_same<Belt, TypeList::type_list_type_at<1, ThirdPassCandidates>::type >::value);
}

namespace full_topological_sort
{
   // Given
   class Trousers;  // Must be worn after underwear.
   class Underwear;
   class Shirt;
   class Shoes;  // Must be worn after socks and trousers.
   class Socks;
   class Belt;   // Must be worn after trousers and shirt.
   class Jacket; // Must be worn after shirt.

   using SampleTypes = TypeList::type_list<Trousers, Underwear, Shirt, Shoes, Socks, Belt, Jacket>;
   using Dependencies = TypeList::type_list<
                           TypeList::type_list<Underwear>,
                           TypeList::type_list<>,
                           TypeList::type_list<>,
                           TypeList::type_list<Socks, Trousers>,
                           TypeList::type_list<>,
                           TypeList::type_list<Trousers, Shirt>,
                           TypeList::type_list<Shirt>
                        >;

   // When
   using SortedList = topological_sort<SampleTypes, Dependencies>::type;

   // Then the list is sorted in dependency order
   constexpr auto expectedSize = 7;
   static_assert(expectedSize == SortedList::size());
   static_assert(std::is_same<Underwear, TypeList::type_list_type_at<0, SortedList>::type >::value);
   static_assert(std::is_same<Shirt, TypeList::type_list_type_at<1, SortedList>::type >::value);
   static_assert(std::is_same<Socks, TypeList::type_list_type_at<2, SortedList>::type >::value);
   static_assert(std::is_same<Trousers, TypeList::type_list_type_at<3, SortedList>::type >::value);
   static_assert(std::is_same<Jacket, TypeList::type_list_type_at<4, SortedList>::type >::value);
   static_assert(std::is_same<Shoes, TypeList::type_list_type_at<5, SortedList>::type >::value);
   static_assert(std::is_same<Belt, TypeList::type_list_type_at<6, SortedList>::type >::value);
}

namespace another_full_topological_sort
{
   // Given
   class Alpha;
   class Beta;
   class Gamma;

   using SampleTypes = TypeList::type_list<Beta, Alpha, Gamma>;
   // Arbitrarily, let's say that double depends on float, int depends on char,
   // and long depends on int and char
   using Dependencies = TypeList::type_list<TypeList::type_list<Alpha>, TypeList::type_list<>, TypeList::type_list<Alpha, Beta>>;

   // When
   using SortedList = topological_sort<SampleTypes, Dependencies>::type;

   // Then the list is sorted in dependency order
   constexpr auto expectedSize = 3;
   static_assert(expectedSize == SortedList::size());
   static_assert(std::is_same<Alpha, TypeList::type_list_type_at<0, SortedList>::type >::value);
   static_assert(std::is_same<Beta, TypeList::type_list_type_at<1, SortedList>::type >::value);
   static_assert(std::is_same<Gamma, TypeList::type_list_type_at<2, SortedList>::type >::value);
}

}
