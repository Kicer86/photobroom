
use czkawka_core::similar_images::SimilarImages;

#[no_mangle]
pub unsafe extern "C" fn CreateSimilarImagesObj() -> *mut ()
{
    return Box::into_raw(Box::new(SimilarImages::new())) as *mut ();
}
