// use std::path::{Path, PathBuf};

fn main() -> miette::Result<()> {
    // let out_dir = std::env::var("OUT_DIR").unwrap();
    // let main_dir = PathBuf::from(std::env::var("CARGO_MANIFEST_DIR").unwrap());

    // 编译 error.h
    let bindings = bindgen::Builder::default()
        .header("emc/gmsdk/include/error.h")
        .parse_callbacks(Box::new(bindgen::CargoCallbacks))
        .generate()
        .expect("Unable to generate bindings");

    bindings
        .write_to_file(std::path::Path::new("src/error.rs"))
        .expect("Couldn't write bindings!");

    // 编译 gmsdk.rs 用于c++
    cxx_build::bridges(vec!["src/lib.rs"])
        .include("emc/gmsdk/include")
        .include("emc")
        .file("emc/wrapper.cpp")
        // .file("emc/cxx.cc")
        .flag_if_supported("-std=c++20")
        .compile("wrapper");

    println!("cargo:rerun-if-changed=src/lib.rs");
    println!("cargo:rerun-if-changed=wrapper/emc/wrapper.h");
    println!("cargo:rerun-if-changed=wrapper/emc/wrapper.cpp");
    println!("cargo:rerun-if-changed=wrapper/emc/gmsdk/include/error.h");
    println!("cargo:rerun-if-changed=wrapper/emc/gmsdk/include/gmapi.h");
    println!("cargo:rerun-if-changed=wrapper/emc/gmsdk/include/gmdef.h");
    println!("cargo:rerun-if-changed=wrapper/emc/gmsdk/include/strategy.h");
    println!("cargo:rustc-flags=-L wrapper/emc/gmsdk/lib/win64 -l gmsdk");

    // let gmsdk_lib_dir = Path::new(&main_dir)
    //     .join("emc")
    //     .join("gmsdk")
    //     .join("lib")
    //     .join("win64");
    // dbg!(&gmsdk_lib_dir);
    // println!(
    //     "cargo:rustc-link-search=native={}",
    //     &gmsdk_lib_dir.to_str().unwrap()
    // );
    // println!("cargo:rustc-link-lib=dylib=gmsdk");
    // println!("cargo:rustc-link-lib=static=wrapper");
    Ok(())
}
