#ifndef Analysis_IniSelect_TSGlobals_CombinationShuffler_H
#define Analysis_IniSelect_TSGlobals_CombinationShuffler_H

#include <algorithm>
#include <vector>

/// `NextCombination` is a helper function that allows you to pick \f$n\f$ combinations from an object that has an iterator (e.g. `std::string`, `std::list`, or `std::vector`).
/// @warning The iterator positions will be scrambled after you perform this function!
/// @see [This discussion on StackOverflow](https://stackoverflow.com/questions/127704/algorithm-to-return-all-combinations-of-k-elements-from-n)
/// @author Remco de Boer 雷穆克 (remco.de.boer@ihep.ac.cn or r.e.deboer@students.uu.nl)
/// @author Credits: Mark Nelson http://marknelson.us
/// @date April 17th, 2019

/// @addtogroup BOSS_globals
/// @{

namespace TSGlobals
{
  namespace CombinationShuffler
  {
    template <typename Iter_t>
    bool NextCombination(const Iter_t first, Iter_t k, const Iter_t last)
    {
      if((first == last) || (first == k) || (last == k)) return false;
      Iter_t i1 = first;
      Iter_t i2 = last;
      ++i1;
      if(last == i1) return false;
      i1 = last;
      --i1;
      i1 = k;
      --i2;
      while(first != i1)
      {
        if(*--i1 < *i2)
        {
          Iter_t j = k;
          while(!(*i1 < *j)) ++j;
          std::iter_swap(i1, j);
          ++i1;
          ++j;
          i2 = k;
          std::rotate(i1, j, last);
          while(last != j)
          {
            ++j;
            ++i2;
          }
          std::rotate(k, i2, last);
          return true;
        }
      }
      std::rotate(first, k, last);
      return false;
    }

    template <typename Iter_t>
    bool NextCombination(const Iter_t first, const Iter_t last, const std::size_t& nCombinations)
    {
      return NextCombination(first, first + nCombinations, last);
    }

    template <typename T>
    bool NextCombination(std::vector<T>& vec, const std::size_t& nCombinations)
    {
      return NextCombination(vec.begin(), vec.end(), nCombinations);
    }
  } // namespace CombinationShuffler
} // namespace TSGlobals

/// @}
#endif