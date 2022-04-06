
#include "czkawka_interface.h"

#include "similar_images.hpp"


namespace
{

}


struct SimilarImages::Impl
{
    Impl()
        : m_similarImages(CreateSimilarImagesObj(), &DestroySimilarImagesObj)
    {

    }

    std::unique_ptr<void, void (*)(void *)> m_similarImages;
};


SimilarImages::SimilarImages()
    : m_impl(std::make_unique<Impl>())
{

}


SimilarImages::~SimilarImages()
{

}
