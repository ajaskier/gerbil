vole_module_name("test")
vole_module_description("Gerbil test module")
vole_module_variable("Gerbil_Test")

vole_add_required_dependencies("BOOST" "BOOST_PROGRAM_OPTIONS" "TBB" "OPENCV"
    "QT" "QT_OPENGL" "OPENGL" "Threads")
vole_add_required_dependencies("BOOST_THREAD" "BOOST_CHRONO" "BOOST_DATE_TIME")
vole_add_required_modules("core")

vole_compile_library(
    main
    controller
    subscription_manager
    subscription_utils
    task_scheduler
    model/model
    model/model_a
    model/model_b
    multitype
    worker_thread
)

vole_moc_library(
    controller.h
    subscription_manager.h
    task_scheduler.h
    model/model.h
    model/model_a.h
    model/model_b.h
    multitype.h
    worker_thread.h
)

vole_add_executable("subscriptions" "main")

vole_add_module()
