vole_module_name("subscriptiongui")
vole_module_description("Subscription Manager && Task Scheduler GUI")
vole_module_variable("Gerbil_Subscription_GUI")

vole_add_required_modules("subscription")

vole_compile_library(
        main
        mainwindow
)

vole_moc_library(
        mainwindow.h
)

vole_ui_library(
        mainwindow.ui
)

vole_add_executable("subscriptiongui" "main")

vole_add_module()
