vole_module_name("subscriptiongui")
vole_module_description("Subscription Manager && Task Scheduler GUI")
vole_module_variable("Gerbil_Subscription_GUI")

vole_add_required_dependencies("BOOST" "OPENCV" "QT")
vole_add_required_modules("gerbil_gui" "subscription")


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

        labels/labeldock
        labels/banddock
        labels/bandview
        labels/modewidget

		clustering/clusteringdock

        roi/roidock
        roi/roiview
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

       labels/labeldock.h
       labels/banddock.h
       labels/bandview.h
       labels/modewidget.h

	   clustering/clusteringdock.h

       roi/roidock.h
       roi/roiview.h
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

        labels/labeldock.ui
        labels/banddock.ui
        labels/modewidget.ui

		clustering/clusteringdock.ui

        roi/roidock.ui
        roi/roidock_buttons.ui
)

vole_add_resources(
        gerbil.qrc
)

vole_add_executable("subscriptiongui" "main")

vole_add_module()
