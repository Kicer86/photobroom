// Simple Rust hello world library

use std::ffi::CString;
use std::os::raw::c_char;

#[no_mangle]
pub extern "C" fn rust_hello_world() -> *const c_char {
    let hello = CString::new("Hello from Rust!").unwrap();
    hello.into_raw()
}

#[no_mangle]
pub extern "C" fn rust_free_string(s: *mut c_char) {
    unsafe {
        if !s.is_null() {
            drop(CString::from_raw(s));
        }
    }
}
