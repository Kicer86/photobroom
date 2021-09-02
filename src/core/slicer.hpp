
#ifndef SLICER_HPP_INCLUDED
#define SLICER_HPP_INCLUDED


template<std::random_access_iterator T, typename O>
void slice(T first, T last, long chunkSize, O o)
    requires std::is_invocable_v<O, T, T>
{
    while(first != last)
    {
        const auto maxChunkSize = last - first;
        chunkSize = std::min(chunkSize, maxChunkSize);

        auto from = first;
        auto to = first + chunkSize;

        o(from, to);

        first += chunkSize;
    }
}

#endif
