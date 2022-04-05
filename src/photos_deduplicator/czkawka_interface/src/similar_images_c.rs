
use czkawka_core::similar_images::SimilarImages;

#[no_mangle]
pub extern "C" fn CreateSimilarImagesObj()
{
    let sf = SimilarImages::new();
}
