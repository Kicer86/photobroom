
#ifndef SIMILAR_IMAGES_HPP_INCLUDED
#define SIMILAR_IMAGES_HPP_INCLUDED

#include <memory>

class SimilarImages final
{
public:
    SimilarImages();
    ~SimilarImages();

    void findSimilarImages();

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

#endif
