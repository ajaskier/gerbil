vole_module_name("task_order_test")
vole_module_description("task_order_test")
vole_module_variable("Gerbil_task_order_test")

vole_add_required_dependencies("BOOST" "TBB" "OPENCV" "QT" "Threads")
vole_add_required_modules("dynamic_any" "subscription" "test")

vole_compile_library(
  task_order_test
)

vole_moc_library(
  task_order_test.h
)

vole_add_test("task_order_test" "task_order_test")


vole_add_module()
