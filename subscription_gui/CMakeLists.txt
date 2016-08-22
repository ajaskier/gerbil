vole_module_name("subscriptiongui")
vole_module_description("Subscription Manager && Task Scheduler GUI")
vole_module_variable("Gerbil_Subscription_GUI")

vole_add_required_modules("subscription")

vole_compile_library(
        main
        mainwindow
        normdock
)

vole_moc_library(
        mainwindow.h
        normdock.h
)

vole_ui_library(
        mainwindow.ui
        normdock.ui
)

vole_add_executable("subscriptiongui" "main")

vole_add_module()
