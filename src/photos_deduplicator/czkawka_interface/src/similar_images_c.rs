
use czkawka_core::similar_images::SimilarImages;


pub struct SimilarImagesData
{
    si: SimilarImages,
}

#[no_mangle]
pub unsafe extern "C" fn createSimilarImagesObj() -> *mut SimilarImagesData
{
    return Box::into_raw(Box::new( SimilarImagesData{si: SimilarImages::new()} ));
}


#[no_mangle]
pub unsafe extern "C" fn destroySimilarImagesObj(sim_img: &mut SimilarImagesData)
{
    let _ = Box::from_raw(sim_img);
}


#[no_mangle]
pub unsafe extern "C" fn collectImages(sim_img: &mut SimilarImagesData)
{
    sim_img.si.find_similar_images(None, None);
}
