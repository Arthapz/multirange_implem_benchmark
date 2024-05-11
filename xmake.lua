add_requires("nanobench")

option("optimize", { default = true })
option("std_tuple", { default = true })

if is_mode("release") then
	add_cxxflags("clang::-fexperimental-library")
  set_optimize("fastest")
  add_cxflags("clang::-march=native")
  add_cxflags("gcc::-march=native", "gcc::-mtune=native")
  add_vectorexts("avx", "avx2")
  add_vectorexts("sse", "sse2", "sse3", "ssse3", "sse4.2")
  set_policy("build.optimization.lto", true)
else
  set_policy("build.sanitizer.address", true)
  set_policy("build.sanitizer.undefined", true)
  set_policy("build.sanitizer.leak", true)
  add_ldflags("clang::-fsanitize=address", {force = true})
  add_ldflags("clang::-fsanitize=undefined", {force = true})
  add_ldflags("clang::-fsanitize=leak", {force = true})
end

if not has_config("std_tuple") then
	add_requires("tuplet")
	add_defines("NO_STD_TUPLE")
end

target("bench", function()
	set_kind("binary")
	set_languages("cxxlatest", "clatest")
	add_packages("nanobench")
	add_files("src/*.cpp")
	add_headerfiles("src/*.hpp")
	if not has_config("std_tuple") then
		add_packages("tuplet")
	end
end)
