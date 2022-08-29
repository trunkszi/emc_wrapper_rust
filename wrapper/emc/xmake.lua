--add_rules("mode.debug", "mode.release")
add_rules("mode.debug")

target("wrapper")
    set_kind("static")
    set_arch("x64")
    set_languages("c++20")
    add_files("*.cpp")
    add_includedirs("../../","gmsdk/include", "../../target/cxxbridge")
    if is_plat("windows") then
        set_symbols("debug")
        set_runtimes("MTd")
        add_ldflags("/DEFAULTLIB:libcmtd")
        add_syslinks("libcmtd")
        add_linkdirs("gmsdk/lib/win64")
    end
    add_links("gmsdk")


target("demo")
    set_kind("binary")
    set_arch("x64")
    set_languages("c++20")
    add_files("*.cpp")
    add_includedirs("gmsdk/include", "../../target/cxxbridge")
    if is_plat("windows") then
        set_symbols("debug")
        set_runtimes("MTd")
        add_ldflags("/DEFAULTLIB:libcmtd")
        add_syslinks("libcmtd")
        add_linkdirs("gmsdk/lib/win64")
    end
    add_links("gmsdk")
    add_deps("wrapper")