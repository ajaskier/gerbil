#add_subdirectory( examples )
#install( FILES include/boost/any_ref.hpp 
#               include/boost/dynamic_any.hpp DESTINATION include/boost )

vole_module_name("dynamic_any")
vole_module_description("none")
vole_module_variable("dynamic_any")

vole_add_required_dependencies("BOOST")

vole_compile_library(
        boost/any_ref
        boost/dynamic_any
)

vole_add_module()
