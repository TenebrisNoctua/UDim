add_library(Luwow.UDimCore STATIC
    UDimCore.h
    UDimCore.cpp
)
target_include_directories(Luwow.UDimCore
    PUBLIC ${UDIM_ROOT}/src
    PRIVATE $<TARGET_PROPERTY:Luau.VM,INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Luwow.UDimCore PRIVATE $<TARGET_PROPERTY:Luau.VM,INTERFACE_COMPILE_DEFINITIONS>)

luwow_add_library(Luwow.UDimLibrary
    NAME udim
    MODES SERIAL
    SOURCES
        Library.h
        UDimLibrary.cpp
)
target_link_libraries(Luwow.UDimLibrary PRIVATE Luwow.UDimCore)

luwow_add_library(Luwow.UDim2Library
    NAME udim2
    MODES SERIAL
    SOURCES
        Library.h
        UDim2Library.cpp
)
target_link_libraries(Luwow.UDim2Library PRIVATE Luwow.UDimCore)
