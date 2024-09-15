#!/bin/bash
set -eu
cd "$(dirname "$0")"

# Unpack args
for arg in "$@"; do declare $arg='1'; done
if [ ! -v gcc ];     then clang=1; fi
if [ ! -v release ]; then debug=1; fi
if [ -v debug ];     then echo "[debug mode]"; fi
if [ -v release ];   then echo "[release mode]"; fi
if [ -v clang ];     then compiler="${CC:-clang}"; echo "[clang compile]"; fi
if [ -v gcc ];       then compiler="${CC:-gcc}"; echo "[gcc compile]"; fi

# Defaults
auto_compile_flags=''

# Compile/link line definitions
clang_common='-Isrc/ -g -Wno-unknown-warning-option -fdiagnostics-absolute-paths -Wall -Wno-missing-braces -Wno-unused-function -Wno-writable-strings -Wno-unused-value -Wno-unused-variable -Wno-unused-local-typedef -Wno-deprecated-register -Wno-deprecated-declarations -Wno-unused-but-set-variable -Wno-single-bit-bitfield-constant-conversion -Wno-compare-distinct-pointer-types -Wno-initializer-overrides -Wno-incompatible-pointer-types-discards-qualifiers -Wno-for-loop-analysis -Xclang -flto-visibility-public-std -D_USE_MATH_DEFINES -Dstrdup=_strdup -Dgnu_printf=printf'
clang_debug="$compiler -g -O0 -DBUILD_DEBUG=1 ${clang_common} ${auto_compile_flags}"
clang_release="$compiler -g -O2 -DBUILD_DEBUG=0 ${clang_common} ${auto_compile_flags}"
clang_link="-lpthread -lm -lrt -ldl -lglfw -lGL"
clang_out="-o"
gcc_common='-Isrc/ -g -Wno-unknown-warning-option -Wall -Wno-missing-braces -Wno-unused-function -Wno-attributes -Wno-unused-value -Wno-unused-variable -Wno-unused-local-typedef -Wno-deprecated-declarations -Wno-unused-but-set-variable -Wno-compare-distinct-pointer-types -D_USE_MATH_DEFINES -Dstrdup=_strdup -Dgnu_printf=printf'
gcc_debug="$compiler -g -O0 -DBUILD_DEBUG=1 ${gcc_common} ${auto_compile_flags}"
gcc_release="$compiler -g -O2 -DBUILD_DEBUG=0 ${gcc_common} ${auto_compile_flags}"
gcc_link="-lpthread -lm -lrt -ldl -lglfw -lGL"
gcc_out="-o"

# Chose compile/link lines
if [ -v gcc ];     then compile_debug="$gcc_debug"; fi
if [ -v gcc ];     then compile_release="$gcc_release"; fi
if [ -v gcc ];     then compile_link="$gcc_link"; fi
if [ -v gcc ];     then out="$gcc_out"; fi
if [ -v clang ];   then compile_debug="$clang_debug"; fi
if [ -v clang ];   then compile_release="$clang_release"; fi
if [ -v clang ];   then compile_link="$clang_link"; fi
if [ -v clang ];   then out="$clang_out"; fi
if [ -v debug ];   then compile="$compile_debug"; fi
if [ -v release ]; then compile="$compile_release"; fi

# Prep directory
mkdir -p build

# Find all tests files -> build and run sequentially
function build_then_run_tests_seq() {
  test_files=$(find src/ -type f -name '*_test.c')
  
  while IFS= read -r file; do
      # Remove path and extension
      base_name=$(basename "$file" .c)
      $compile $file $compile_link $out $base_name
      mv $base_name build/$base_name
      ./build/$base_name
  done <<< "$test_files"
}

# Build
# cd build
if [ -v vanna ]; then didbuild=1 && bear -- $compile src/main.c $compile_link $out vanna && mv vanna build/vanna; fi
if [ -v test ]; then didbuild=1 && build_then_run_tests_seq; fi
cd ../

# Warn on no builds
if [ ! -v didbuild ]
then
  echo "[WARNING] no valid build target specified; must use build target names as arguments to this script, like \`./workflow.sh vanna\`."
  exit 1
fi

# Build all tests files



