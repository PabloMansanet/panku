#pragma once

#include <algorithm>
#include <tuple>
#include <type_traits>
#include <functional>
#include <array>

#include "TypeList.h"
#include "TupleManipulation.h"

#define PANKU_LIST(...) \
   using PankuUserProvidedList = TypeList::type_list<__VA_ARGS__>; \
   using PankuProcessedList = PankuMetaprogram::type_list_extract_heads<PankuUserProvidedList>; \
   using PankuUserClassList = PankuProcessedList::type; \
   using PankuDependencies = PankuProcessedList::headless_type; \
   using PankuPath = PankuMetaprogram::topological_sort<PankuUserClassList, PankuDependencies>::type; \
   using PankuPathPointerised = PankuMetaprogram::type_list_pointerise<PankuPath>::type; \
   using PankuClassTuple = TypeList::convert<PankuPathPointerised, std::tuple>; \
   class panku \
   { \
   public: \
      panku() \
      { \
         static bool unique = true; \
         if (!unique) \
            for(;;){}; \
         unique = false; \
      } \
      void Initialise()  \
      { \
         mInitialised = true; \
         TupleManipulation::for_each_in_tuple(userClassTuple, [](auto& element) {  \
               PankuMetaprogram::EntryFactory<decltype(*element)> factory; \
               element = &factory.ConstructEntry();  \
            }); \
      } \
      template<typename UserClass, int N = 0> \
      UserClass& Get()  \
      { \
         if (!mInitialised) \
            Initialise(); \
         PankuMetaprogram::TupleAccessor<UserClass, PankuClassTuple, N> accessor; \
         auto userObjectPointer = accessor.Get(userClassTuple); \
         if (!userObjectPointer) \
            for(;;); \
         return *userObjectPointer; \
      } \
      template<typename UserClass> \
      void ForEach(std::function<void(UserClass&)> f) \
      { \
         TupleManipulation::for_each_in_tuple(userClassTuple, [f](auto element) {  \
               PankuMetaprogram::ConditionalFunctor<typename std::remove_pointer<decltype(element)>::type, UserClass, decltype(f)> (*element, f); \
            }); \
      } \
   private: \
      PankuClassTuple userClassTuple; \
      bool mInitialised; \
   }; \

#define DEPENDENCY(ClassName, ...) TypeList::type_list<ClassName, ##__VA_ARGS__>
#define COLLECTION(number, ClassName, ...) TypeList::type_list<PankuMetaprogram::Collection<ClassName, number>, ##__VA_ARGS__>

template<typename UserClass, int N = 0>
UserClass& ConstructAndInitialise();

namespace PankuMetaprogram
{
   template<class UserClass, int N>
   struct Collection
   {
      using CollectionType = UserClass;
      static constexpr int Size = N;

      Collection():
         collectionArray({0})
      {
         ConstructCollectionElement<0>();
      }
      std::array<UserClass*, N> collectionArray;

      template<int P>
      inline typename std::enable_if<(P >= (N-1)), void>::type ConstructCollectionElement()
      {
         collectionArray[P] = &ConstructAndInitialise<UserClass&, P>();
      }

      template<int P>
      inline typename std::enable_if<(P < (N-1)), void>::type ConstructCollectionElement()
      {
         ConstructCollectionElement<P+1>();
         collectionArray[P] = &ConstructAndInitialise<UserClass&, P>();
      }

      UserClass* GetCollectionElement(int position) {
         return collectionArray.at(position);
      }
   };

   template <typename T>
   struct is_collection
   {
      static constexpr bool value = false;
   };

   template <typename T, int N>
   struct is_collection<Collection<T, N>>
   {
      static constexpr bool value = true;
   };

   // Case where the element is on a collection
   template<class UserClass, class UserClassTuple, int N = 0, typename specialization = void>
   struct TupleAccessor
   {
      UserClass* Get(UserClassTuple userClassTuple) 
      {
         UserClass* userObject = 0;
         TupleManipulation::for_each_in_tuple(userClassTuple, [&](auto element) {
            if (!userObject)
               userObject = RetrieveFromCollection(*element);
         }); 

         return userObject;
      }

      template<class ElementType>
      inline typename std::enable_if<!is_collection<ElementType>::value, UserClass*>::type RetrieveFromCollection(ElementType)
      {
         return 0;
      }

      template<class ElementType>
      inline typename std::enable_if<!std::is_same<typename ElementType::CollectionType, UserClass>::value, UserClass*>::type RetrieveFromCollection(ElementType)
      {
         return 0;
      }

      template<class ElementType>
      inline typename std::enable_if<std::is_same<typename ElementType::CollectionType, UserClass>::value, UserClass*>::type RetrieveFromCollection(ElementType element)
      {
         static_assert(ElementType::Size > N, "You are attempting to extract out-of-bounds elements from a Panku collection!" );
         return element.GetCollectionElement(N);
      }
   };

   // Case where the element is alone
   template<class UserClass, class UserClassTuple, int N>
   struct TupleAccessor<UserClass, UserClassTuple, N, typename std::enable_if<TupleManipulation::has_type<UserClass*, UserClassTuple>::value>::type >
   {
      UserClass* Get(UserClassTuple userClassTuple) 
      {
         return std::get<UserClass*>(userClassTuple);
      }
   };


   template<class PankuEntry>
   struct EntryFactory
   {
      PankuEntry& ConstructEntry()
      {
         return ConstructAndInitialise<PankuEntry, 0>();
      }
   };

   template<class UserClass, int N>
   struct EntryFactory<Collection<UserClass, N>&>
   {
      Collection<UserClass, N>& ConstructEntry()
      {
         static Collection<UserClass, N> collection;
         return collection;
      }
   };

   // Applies the functor if Child is derived from Parent
   // The base case (where Child is NOT derived) does nothing
   template<class Child, class Parent, typename Functor>
   inline typename std::enable_if<!is_collection<Child>::value && !std::is_base_of<Parent, Child>::value, void>::type
   ConditionalFunctor(Child&, Functor)
   {}

   // Case where element is a collection, we must dig into it
   template<class Child, class Parent, typename Functor>
   inline typename std::enable_if<is_collection<Child>::value && std::is_base_of<Parent, typename Child::CollectionType>::value, void>::type
   ConditionalFunctor(Child& collection, Functor f)
   {
      for (auto iterator = collection.collectionArray.begin(); iterator != collection.collectionArray.end(); iterator++) {
         f(**iterator);
      }
   }

   template<class Child, class Parent, typename Functor>
   inline typename std::enable_if<std::is_base_of<Parent, Child>::value, void>::type
   ConditionalFunctor(Child& childObject, Functor functor)
   {
      functor(childObject);
   }

   // Turns a type list into a type list of its pointer types.
   template<class List>
   struct type_list_pointerise;

   template<>
   struct type_list_pointerise<TypeList::type_list<>>
   {
      using type = TypeList::type_list<>;
   };

   template<typename T, typename... Ts>
   struct type_list_pointerise<TypeList::type_list<T, Ts...>>
   {
      using type = typename TypeList::type_list_push_front<T*,
         typename type_list_pointerise<TypeList::type_list<Ts...>>::type>::type;
   };

   // Extracts the first type from each list in a list of lists
   template <class ListOfLists>
   struct type_list_extract_heads;

   template<>
   struct type_list_extract_heads<TypeList::type_list<>>
   {
      using type = TypeList::type_list<>;
      using headless_type = TypeList::type_list<>;
   };

   template<typename T, typename... OtherLists>
   struct type_list_extract_heads<TypeList::type_list<TypeList::type_list<T>, OtherLists...>>
   {
      using type = typename TypeList::type_list_push_front<T,
         typename type_list_extract_heads<TypeList::type_list<OtherLists...>>::type>::type;

      using headless_type = typename TypeList::type_list_push_front<TypeList::type_list<>,
         typename type_list_extract_heads<TypeList::type_list<OtherLists...>>::headless_type>::type;
   };

   template<typename T, typename... Ts, typename... OtherLists>
   struct type_list_extract_heads<TypeList::type_list<TypeList::type_list<T, Ts...>, OtherLists...>>
   {
      using type = typename TypeList::type_list_push_front<T,
         typename type_list_extract_heads<TypeList::type_list<OtherLists...>>::type>::type;

      using headless_type = typename TypeList::type_list_push_front<TypeList::type_list<Ts...>,
         typename type_list_extract_heads<TypeList::type_list<OtherLists...>>::headless_type>::type;
   };

   // Check all elements of list A are in list B //
   template<typename ListA, typename ListB>
   struct check_all_in;

   // By default, all elements in an empty list are in another list
   template<typename ListB>
   struct check_all_in<TypeList::type_list<>, ListB>
   {
      using type = std::true_type;
      using value_type = type::value_type;
      static constexpr value_type value = type::value;
   };

   // Recursive case
   template<typename T, typename... Ts, typename ListB>
   struct check_all_in<TypeList::type_list<T, Ts...>, ListB>
   {
   private:
      static constexpr bool TInListB = TypeList::type_list_index_of<T, ListB>::value != -1;
   public:
      using type = typename std::conditional < TInListB,
                      typename check_all_in<TypeList::type_list<Ts...>, ListB>::type,
                      std::integral_constant<bool, false >
                   >::type;

      using value_type = typename type::value_type;
      static constexpr value_type value = type::value;
   };

   // Constructs list of nodes that are not yet in the path, but whose dependencies
   // are in the path
   template<typename UserClassList, typename UserClassDependencies, typename Path>
   struct extract_candidates;

   template<typename ListB, typename Path>
   struct extract_candidates<TypeList::type_list<>, ListB, Path>
   {
      using type = TypeList::type_list<>;
   };

   template<typename T, typename Path, typename... Ts, typename D, typename... Ds>
   struct extract_candidates<TypeList::type_list<T, Ts...>, TypeList::type_list<D, Ds...>, Path>
   {
   private:
      static constexpr bool UserClassInPath = TypeList::type_list_index_of<T,Path>::value != -1;
      static constexpr bool AllUserClassDependenciesInPath = check_all_in<D, Path>::value;
      static constexpr bool UserClassIsCandidate = !UserClassInPath && AllUserClassDependenciesInPath;
      using next_recursive_step = typename extract_candidates<TypeList::type_list<Ts...>,
         TypeList::type_list<Ds...>, Path>::type;

   public:

      // If all conditions hold, our candidates list consists of the front element T plus
      // the recursive application of the algorithm to the remainder of the list. Otherwise
      // T is skipped
      using type = typename std::conditional < UserClassIsCandidate,
                      typename TypeList::type_list_push_front<T, next_recursive_step>::type,
                      next_recursive_step
                   >::type;

   };

   template <typename UserClassList, typename UserClassDependencies, typename Path, typename Specialization = void>
   struct topological_sort_step
   {
   private:
      using Candidates = typename extract_candidates<UserClassList, UserClassDependencies, Path>::type;
      using NewPath = typename TypeList::type_list_concat_lists<Path, Candidates>::type;
      static constexpr bool Finished = UserClassList::size() == NewPath::size();
      static constexpr bool Stuck = Candidates::size() == 0;
      static_assert(Finished || !Stuck, "Panku dependency graph is not solvable."
                                        " Please beware of circular dependencies");

   public:
      using type = typename topological_sort_step<UserClassList, UserClassDependencies, NewPath>::type;

   };

   // SFINAE switch for termination condition
   // (when the Path includes all elements in the user class list)
   template<typename UserClassList, typename UserClassDependencies, typename Path>
   struct topological_sort_step<UserClassList,
                                UserClassDependencies,
                                Path,
                                typename std::enable_if<UserClassList::size() == Path::size()>::type>
   {
      using type = typename Path::type;
   };

   // Compile time topological sort
   // Attempts to construct a path through each of the user typenamees, so that each is
   // initialised after all of its dependencies.
   template <typename UserClassList, typename UserClassDependencies>
   struct topological_sort
   {
   public:
      using type = typename topological_sort_step<UserClassList,
                      UserClassDependencies,
                      TypeList::type_list<>
                   >::type;
   };
}
#include "MetaprogramTests.h"
