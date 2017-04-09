vole_module_name("test")
vole_module_description("Gerbil tests")
vole_module_variable("Gerbil_Test")

vole_add_required_dependencies("BOOST" "OPENCV" "QT")
vole_add_required_modules("subscription")

vole_compile_library(
  task_scheduler_mock
  data_task_chain
  task_test
  task_order_test
)

vole_moc_library(
  task_scheduler_mock.h
  task_test.h
  task_order_test.h
)

vole_add_test("task_test" "task_test")
vole_add_test("task_order_test" "task_order_test")

vole_add_module()
