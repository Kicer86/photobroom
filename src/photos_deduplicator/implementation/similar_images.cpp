
#include "czkawka_interface.h"

#include "similar_images.hpp"


namespace
{

}


struct SimilarImages::Impl
{
    Impl()
        : m_similarImages(createSimilarImagesObj(), &destroySimilarImagesObj)
    {

    }

    std::unique_ptr<SimilarImagesData, void (*)(SimilarImagesData *)> m_similarImages;
};


SimilarImages::SimilarImages()
    : m_impl(std::make_unique<Impl>())
{

}


SimilarImages::~SimilarImages()
{

}


void SimilarImages::findSimilarImages()
{
    collectImages(m_impl->m_similarImages.get());
}
