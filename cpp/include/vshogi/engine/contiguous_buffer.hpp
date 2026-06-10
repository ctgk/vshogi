#ifndef VSHOGI_ENGINE_CONTIGUOUS_BUFFER_HPP
#define VSHOGI_ENGINE_CONTIGUOUS_BUFFER_HPP

#include <utility>
#include <vector>

namespace vshogi::engine
{

template <class T>
class ContiguousBuffer
{
public:
    ContiguousBuffer(const uint n);
    ~ContiguousBuffer() = default; // 1/5 destructor
    ContiguousBuffer(const ContiguousBuffer& other)
        = delete; // 2/5 copy constructor
    ContiguousBuffer& operator=(const ContiguousBuffer& other)
        = delete; // 3/5 copy assignment
    ContiguousBuffer(ContiguousBuffer&& other) = delete; // 4/5 move constructor
    ContiguousBuffer& operator=(ContiguousBuffer&& other)
        = delete; // 5/5 move assignment

    void init();
    template <class... Args>
    T* emplace_next(Args&&... args);

    // clang-format off
    bool is_full() const { return m_next == cend(); }
    auto size() const { return m_next - m_buffer.data(); }
    uint remaining() const { return static_cast<uint>(m_buffer.size()) - static_cast<uint>(m_next - m_buffer.data()) - 1u; }
    T& front() { return m_buffer.front(); }
    const T& front() const { return m_buffer.front(); }
    const T* cend() const { return &m_buffer.back(); }
    T* data() { return m_buffer.data(); }
    const T* cdata() const { return m_buffer.data(); }
    T*& next() { return m_next; }
    // clang-format on

private:
    std::vector<T> m_buffer;
    T* m_next;
};

template <class T>
inline ContiguousBuffer<T>::ContiguousBuffer(const uint n)
    : m_buffer(n + 1u + T::can_be_root()), m_next{} // +2u for begin and end
{
    init();
}

template <class T>
inline void ContiguousBuffer<T>::init()
{
    m_buffer.front().init();
    m_next = m_buffer.data();
    if constexpr (T::can_be_root()) {
        ++m_next;
        m_next->init();
    }
    m_buffer.back().init_as_end();
}

template <class T>
template <class... Args>
T* ContiguousBuffer<T>::emplace_next(Args&&... args)
{
    if (is_full())
        return nullptr;
    T* out = m_next++;
    out->init(std::forward<Args>(args)...);
    if (m_next != &m_buffer.back())
        m_next->init();
    return out;
}

} // namespace vshogi::engine

#endif // VSHOGI_ENGINE_CONTIGUOUS_BUFFER_HPP
