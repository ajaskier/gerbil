vole_module_name("test")
vole_module_description("Gerbil tests")
vole_module_variable("Gerbil_Test")

vole_add_required_dependencies("BOOST" "TBB" "OPENCV" "QT" "Threads")
vole_add_required_modules("dynamic_any" "subscription")

vole_compile_library(
  fake/tst_faketest
)

vole_moc_library(
  fake/tst_faketest.h
)

vole_add_test("faketest" "fake/tst_faketest")

vole_add_module()
