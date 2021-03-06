vole_module_name("gerbil_gui")
vole_module_description("GUI for multispectral images")
vole_module_variable("Gerbil_GUI")

vole_add_required_dependencies("BOOST" "BOOST_PROGRAM_OPTIONS" "TBB" "OPENCV"
    "QT" "QT_OPENGL" "OPENGL" "Threads")
vole_add_required_dependencies("BOOST_THREAD" "BOOST_CHRONO" "BOOST_DATE_TIME")
vole_add_required_modules("rgb" "seg_graphs")
vole_add_optional_modules("seg_meanshift" "seg_medianshift" "seg_probshift" "edge_detect")

vole_compile_library(
	app/gerbilapplication
	app/gerbilapp_aux
	app/gerbilio

	widgets/actionbutton
	widgets/mainwindow
	widgets/autohidewidget
	widgets/autohideview
	widgets/ahcombobox
	widgets/scaledview
	widgets/bandview
	widgets/roiview
	widgets/graphsegwidget
	widgets/modewidget
	widgets/similaritywidget

	widgets/sizegripitem/sizegripitem
	
	controller/controller
	controller/controller_docks
	controller/distviewcontroller
	controller/subscriptions
	# Huh? module not compiled otherwise... weird.
	controller/subscriptions.cpp

	dist_view/distviewcompute
	dist_view/distviewmodel
	dist_view/viewer_tasks
	dist_view/foldingbar
	dist_view/viewport
	dist_view/viewport_drawing
	dist_view/viewport_input
	dist_view/distviewgui
	dist_view/distviewbinstbb

	model/commandrunner
	model/representation
	model/imagemodel
	model/labelingmodel
	model/falsecolormodel
	model/falsecolor/falsecoloring
	model/falsecolor/falsecolormodelpayload
	model/illuminationmodel
	model/graphsegmentationmodel
	model/clusteringmodel
	model/labels/icontask

	docks/banddock
	docks/normdock
	docks/roidock
	docks/illumdock
	docks/falsecolordock
	docks/clusteringdock
	docks/labeldock

	gerbil_gui_debug

	dialogs/openrecent/recentfile
	dialogs/openrecent/openrecent
	dialogs/openrecent/recentfiledelegate
)

vole_moc_library(
	app/gerbilapplication.h

	widgets/mainwindow.h
	widgets/actionbutton.h
	widgets/autohidewidget.h
	widgets/autohideview.h
	widgets/ahcombobox.h
	widgets/scaledview.h
	widgets/bandview.h
	widgets/roiview.h
	widgets/graphsegwidget.h
	widgets/modewidget.h
	widgets/similaritywidget.h

	widgets/sizegripitem/sizegripitem.h

	controller/controller.h
	controller/distviewcontroller.h

	dist_view/distviewmodel.h
	dist_view/distviewgui.h
	dist_view/foldingbar.h
	dist_view/viewport.h

	docks/banddock.h
	docks/normdock.h
	docks/roidock.h
	docks/illumdock.h
	docks/falsecolordock.h
	docks/clusteringdock.h
	docks/labeldock.h

	model/commandrunner.h
	model/imagemodel.h
	model/labelingmodel.h
	model/labels/icontask.h
	model/falsecolormodel.h
	model/falsecolor/falsecolormodelpayload.h
	model/illuminationmodel.h
	model/graphsegmentationmodel.h
	model/clusteringmodel.h

	dialogs/openrecent/openrecent.h
	dialogs/openrecent/recentfiledelegate.h
)

vole_ui_library(
	widgets/mainwindow.ui
	widgets/graphsegwidget.ui
	widgets/modewidget.ui
	widgets/similaritywidget.ui
	dist_view/distviewgui.ui
	dist_view/foldingbar.ui
	dist_view/viewportcontrol.ui
	docks/roidock.ui
	docks/roidock_buttons.ui
	docks/illumdock.ui
	docks/clusteringdock.ui
	docks/banddock.ui
	docks/normdock.ui
	docks/falsecolordock_sel.ui
	docks/labeldock.ui
	dialogs/openrecent/openrecent.ui
)

vole_add_resources(
	gerbil.qrc
)

vole_add_executable("qgerbil" "app/qgerbilmain")


vole_add_module()

