vole_module_name("test")
vole_module_description("Gerbil tests")
vole_module_variable("Gerbil_Test")

vole_add_required_dependencies("BOOST" "OPENCV" "QT")
vole_add_required_modules("subscription")

vole_compile_library(
  task_scheduler_mock
  data_task_chain

  test_task_core
  test_multi_data_subscriptions
  test_random_data_subscription

  task_order_test
)

vole_moc_library(
  task_scheduler_mock.h

  test_task_core.h
  test_multi_data_subscriptions.h
  test_random_data_subscription.h

  task_order_test.h
)

vole_add_test("task_order_test" "task_order_test")
vole_add_test("test_random_data_subscription" "test_random_data_subscription")
vole_add_test("test_multi_data_subscriptions" "test_multi_data_subscriptions")

vole_add_module()
