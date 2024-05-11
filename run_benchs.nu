def run_benchmark_gcc [name: string, ...options] {
  print $"(ansi green_bold)GCC ($name)(ansi reset)"
  rm -rf .xmake build
  xmake f --yes ...$options | save -f /dev/null
  xmake b | save -f /dev/null
  xmake run 
}

def run_benchmark_clang [name: string, ...options] {
  print $"(ansi green_bold)Clang ($name)(ansi reset)"
  rm -rf .xmake build
  xmake f --yes --toolchain=llvm --sdk=/opt/llvm-git/ --runtimes=c++_shared ...$options | save -f /dev/null
  xmake b | save -f /dev/null
  xmake run 
}

run_benchmark_gcc "optimized std tuple" "-m" "release" "--std_tuple=y"
run_benchmark_gcc "optimized tuplet tuple" "-m" "release" "--std_tuple=n"
# run_benchmark_gcc "not optimized std tuple" -m debug --std_tuple=y
# run_benchmark_gcc "not optimized tuplet tuple" -m debug --std_tuple=n

run_benchmark_clang "optimized std tuple" "-m" "release" "--std_tuple=y"
run_benchmark_clang "optimized tuplet tuple" "-m" "release" "--std_tuple=n"
# run_benchmark_clang "not optimized std tuple" -m debug --std_tuple=y
# run_benchmark_clang "not optimized tuplet tuple" -m debug --std_tuple=n

