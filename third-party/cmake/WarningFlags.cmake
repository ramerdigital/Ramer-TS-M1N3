add_library(warning_flags INTERFACE)

if((CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") OR (CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC"))
    target_compile_options(warning_flags INTERFACE
        /W4     # base warning level
        /WX     # Treat warnings as errors
        #/wd4458 # declaration hides class member (from Foley's GUI Magic)
        /wd4505 # since VS2019 doesn't handle [[ maybe_unused ]] for static functions (RTNeural::debug_print)
        /wd4244 # for XSIMD
    )
elseif((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"))
    target_compile_options(warning_flags INTERFACE
        -Werror # Treat warnings as errors
        -Wno-pass-failed # Suppress warnings/errors when compiler fails to auto-vectorize loops (prevents build failure under -Werror and avoids flooding terminal with optimization reports)
        -Wall -Wshadow-all -Wshorten-64-to-32 -Wstrict-aliasing -Wuninitialized
        -Wunused-parameter -Wconversion -Wsign-compare -Wint-conversion
        -Wconditional-uninitialized -Woverloaded-virtual -Wreorder
        -Wconstant-conversion -Wsign-conversion -Wunused-private-field
        -Wbool-conversion -Wunreachable-code
        -Wzero-as-null-pointer-constant -Wcast-align
        -Wshift-sign-overflow -Wnullable-to-nonnull-conversion -Wpedantic
        
        # Suppress warnings for redundant/extra semicolons
        -Wno-extra-semi
        
        # Suppress warnings for missing override keywords in overridden destructors (frequent in JUCE framework headers)
        -Wno-inconsistent-missing-destructor-override
        
        # Suppress warnings when structs are partially initialized (common in configuration structs)
        -Wno-missing-field-initializers
        
        # Suppress warnings when qualifiers like const are ignored on value return types (common in legacy APIs)
        -Wno-ignored-qualifiers
    )

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(warning_flags INTERFACE
        -Werror # Treat warnings as errors
        -Wall -Wextra -Wstrict-aliasing -Wuninitialized -Wunused-parameter
        -Wsign-compare -Woverloaded-virtual -Wreorder -Wunreachable-code
        -Wzero-as-null-pointer-constant -Wcast-align -Wno-implicit-fallthrough
        -Wno-maybe-uninitialized -Wno-missing-field-initializers -Wno-pedantic
        -Wno-ignored-qualifiers -Wno-unused-function -Wno-pessimizing-move
        # From LV2 Wrapper
        -Wno-parentheses -Wno-deprecated-declarations -Wno-redundant-decls
        # For XSIMD
        -Wno-zero-as-null-pointer-constant
        # These lines suppress some custom warnings.
        # Comment them out to be more strict.
        -Wno-redundant-move
    )

    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "7.0.0")
        target_compile_options(warning_flags INTERFACE "-Wno-strict-overflow")
    endif()
endif()
