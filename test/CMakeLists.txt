vole_module_name("test")
vole_module_description("Gerbil test module")
vole_module_variable("Gerbil_Test")

vole_add_required_dependencies("GTEST" "GMOCK")
vole_add_required_dependencies("BOOST" "BOOST_PROGRAM_OPTIONS" "TBB" "OPENCV"
    "QT" "QT_OPENGL" "OPENGL" "Threads")
vole_add_required_dependencies("BOOST_THREAD" "BOOST_CHRONO" "BOOST_DATE_TIME")
vole_add_required_modules("rgb" "seg_graphs")

vole_compile_library(
        main
        first_test
        background_task_queue
        simulation
)

#find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
link_directories(${GTEST_MAIN_LIBRARY})

include_directories(${GMOCK_INCLUDE_DIRS})

vole_moc_library(
    simulation.h
)

vole_add_executable("test" "main")

vole_add_module()

