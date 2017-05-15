#pragma once

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
