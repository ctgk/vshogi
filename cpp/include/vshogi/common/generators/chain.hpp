#ifndef VSHOGI_COMMON_GENERATORS_CHAIN_HPP
#define VSHOGI_COMMON_GENERATORS_CHAIN_HPP

#include "vshogi/common/move.hpp"
#include "vshogi/common/state.hpp"
#include "vshogi/common/utils.hpp"

namespace vshogi
{

template <class P, class... Gens>
class ChainGenerator;

template <class P, class Gen0, class Gen1>
class ChainGenerator<P, Gen0, Gen1>
{
private:
    Gen0 m_gen0;
    Gen1 m_gen1;
    uint m_index; //!< 0: gen0, 1: gen1, 2: end

public:
    ChainGenerator(const State<P>& state)
        : m_gen0{state}, m_gen1{state}, m_index{}
    {
        if (m_gen0)
            return;
        ++m_index;
        if (m_gen1)
            return;
        ++m_index;
    }
    ChainGenerator& operator++()
    {
        switch (m_index) {
        case 0u:
            ++m_gen0;
            if (m_gen0)
                break;
            else if (m_gen1)
                m_index = 1u;
            else
                m_index = 2u;
            break;
        case 1u:
            ++m_gen1;
            if (!m_gen1)
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
            return *m_gen0;
        case 1u:
            return *m_gen1;
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

template <class P, class Gen0, class Gen1, class Gen2>
class ChainGenerator<P, Gen0, Gen1, Gen2>
{
private:
    Gen0 m_gen0;
    Gen1 m_gen1;
    Gen2 m_gen2;
    uint m_index; //!< 0: gen0, 1: gen1, 2: gen2, 3: end

public:
    ChainGenerator(const State<P>& state)
        : m_gen0{state}, m_gen1{state}, m_gen2{state}, m_index{}
    {
        if (m_gen0)
            return;
        ++m_index;
        if (m_gen1)
            return;
        ++m_index;
        if (m_gen2)
            return;
        ++m_index;
    }
    ChainGenerator& operator++()
    {
        switch (m_index) {
        case 0u:
            ++m_gen0;
            if (m_gen0)
                break;
            else if (m_gen1)
                m_index = 1u;
            else if (m_gen2)
                m_index = 2u;
            else
                m_index = 3u;
            break;
        case 1u:
            ++m_gen1;
            if (m_gen1)
                break;
            else if (m_gen2)
                m_index = 2u;
            else
                m_index = 3u;
            break;
        case 2u:
            ++m_gen2;
            if (m_gen2)
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
            return *m_gen0;
        case 1u:
            return *m_gen1;
        case 2u:
            return *m_gen2;
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

#endif // VSHOGI_COMMON_GENERATORS_CHAIN_HPP
