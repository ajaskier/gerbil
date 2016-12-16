vole_module_name("subscriptiongui")
vole_module_description("Subscription Manager && Task Scheduler GUI")
vole_module_variable("Gerbil_Subscription_GUI")

vole_add_required_modules("subscription" "gerbil_gui")
vole_add_required_dependencies("QT" "BOOST" "OPENCV")


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

        dist/foldingbar
        dist/distviewgui2
        dist/viewport2
        #dist/distviewcompute2
        dist/viewport_drawing2
        dist/viewport_input2

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

        dist/foldingbar.h
        dist/distviewgui2.h
        dist/viewport2.h
       # dist/distviewcompute2.h
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

        dist/viewportcontrol.ui
        dist/foldingbar.ui
        dist/distviewgui2.ui
)

vole_add_executable("subscriptiongui" "main")

vole_add_module()
