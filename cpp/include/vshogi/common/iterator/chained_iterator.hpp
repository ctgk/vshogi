#ifndef VSHOGI_COMMON_ITERATOR_CHAINED_ITERATOR_HPP
#define VSHOGI_COMMON_ITERATOR_CHAINED_ITERATOR_HPP

#include "vshogi/common/move.hpp"
#include "vshogi/common/state.hpp"
#include "vshogi/common/utils.hpp"

namespace vshogi
{

template <class P, class... Iters>
class ChainedIterator;

template <class P, class Iter0, class Iter1>
class ChainedIterator<P, Iter0, Iter1>
{
private:
    Iter0 m_iter0;
    Iter1 m_iter1;
    uint m_index; //!< 0: iter0, 1: iter1, 2: end

public:
    ChainedIterator(const State<P>& state)
        : m_iter0{state}, m_iter1{state}, m_index{}
    {
        if (m_iter0)
            return;
        ++m_index;
        if (m_iter1)
            return;
        ++m_index;
    }
    ChainedIterator& operator++()
    {
        switch (m_index) {
        case 0u:
            ++m_iter0;
            if (m_iter0)
                break;
            else if (m_iter1)
                m_index = 1u;
            else
                m_index = 2u;
            break;
        case 1u:
            ++m_iter1;
            if (!m_iter1)
                m_index = 2u;
        default:
            break;
        }
        return *this;
    }
    Move<P> operator*() const
    {
        switch (m_index) {
        case 0u:
            return *m_iter0;
        case 1u:
            return *m_iter1;
        default:
            break;
        }
        return Move<P>();
    }
    operator bool() const
    {
        return m_index != 2u;
    }
};

template <class P, class Iter0, class Iter1, class Iter2>
class ChainedIterator<P, Iter0, Iter1, Iter2>
{
private:
    Iter0 m_iter0;
    Iter1 m_iter1;
    Iter2 m_iter2;
    uint m_index; //!< 0: iter0, 1: iter1, 2: iter2, 3: end

public:
    ChainedIterator(const State<P>& state)
        : m_iter0{state}, m_iter1{state}, m_iter2{state}, m_index{}
    {
        if (m_iter0)
            return;
        ++m_index;
        if (m_iter1)
            return;
        ++m_index;
        if (m_iter2)
            return;
        ++m_index;
    }
    ChainedIterator& operator++()
    {
        switch (m_index) {
        case 0u:
            ++m_iter0;
            if (m_iter0)
                break;
            else if (m_iter1)
                m_index = 1u;
            else if (m_iter2)
                m_index = 2u;
            else
                m_index = 3u;
            break;
        case 1u:
            ++m_iter1;
            if (m_iter1)
                break;
            else if (m_iter2)
                m_index = 2u;
            else
                m_index = 3u;
            break;
        case 2u:
            ++m_iter2;
            if (m_iter2)
                break;
            else
                m_index = 3u;
        default:
            break;
        }
        return *this;
    }
    Move<P> operator*() const
    {
        switch (m_index) {
        case 0u:
            return *m_iter0;
        case 1u:
            return *m_iter1;
        case 2u:
            return *m_iter2;
        default:
            break;
        }
        return Move<P>();
    }
    operator bool() const
    {
        return m_index != 3u;
    }
};

} // namespace vshogi

#endif // VSHOGI_COMMON_ITERATOR_CHAINED_ITERATOR_HPP
