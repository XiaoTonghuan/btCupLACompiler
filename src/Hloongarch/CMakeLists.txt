add_library(Hloongarch STATIC
    H2L.cpp
    HLBlock.cpp
    HLFunction.cpp
    HLInst.cpp
    HLLoc.cpp
    HLModule.cpp
    HLValue.cpp
)

target_link_libraries(Hloongarch IR_lib)
