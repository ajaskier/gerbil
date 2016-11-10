vole_module_name("subscriptiongui")
vole_module_description("Subscription Manager && Task Scheduler GUI")
vole_module_variable("Gerbil_Subscription_GUI")

vole_add_required_modules("subscription")

vole_compile_library(
        main
        mainwindow
        imgwindow
        normdock
        awindow
        bwindow
        cwindow
        bandswindow
        distwindow
)

vole_moc_library(
        mainwindow.h
        imgwindow.h
        normdock.h
        awindow.h
        bwindow.h
        cwindow.h
        bandswindow.h
        distwindow.h
)

vole_ui_library(
        mainwindow.ui
        imgwindow.ui
        normdock.ui
        awindow.ui
        bwindow.ui
        cwindow.ui
        bandswindow.ui
        distwindow.ui
)

vole_add_executable("subscriptiongui" "main")

vole_add_module()
