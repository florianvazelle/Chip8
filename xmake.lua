-- project
set_project("chip8")

-- set xmake minimum version
set_xmakever("2.5.8")

-- set project version
set_version("1.0,0", {build = "%Y%m%d%H%M"})

-- set warning all as error
set_warnings("all", "error")

-- set language: c++17
set_languages("c++17")

-- add build modes
add_rules("mode.debug", "mode.release")

-- the debug mode
if is_mode("debug") then
    
    -- enable the debug symbols
    set_symbols("debug")

    -- disable optimization
    set_optimize("none")
end

-- add required dependencies
add_requires("opengl")
add_requires("glfw") -- Window / Cursor / Event libary
add_requires("glew") -- OpenGL loader
add_requires("openal-soft") -- Audio
add_requires("cxxopts") -- Command-line parser

-- add a dependencies lock file
set_policy("package.requires_lock", true)

-- set a license (for openal-soft)
set_license("LGPL-2.0")

-- target
target("chip8")
    set_kind("binary")
    add_files("src/*.cpp")
    add_includedirs("include/")
    add_packages("opengl", "glfw", "glew", "openal-soft", "cxxopts")
