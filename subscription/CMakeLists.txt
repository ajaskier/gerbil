vole_module_name("subscription")
vole_module_description("Subscription Manager && Task Scheduler functionality")
vole_module_variable("Gerbil_Subscription")

vole_add_required_modules("gerbil_gui" "dynamic_any")
vole_add_required_dependencies("QT" "BOOST" "TBB" "OPENCV")

vole_compile_library(
	data
	data_entry
	dependency
	lock
	state
	subscription
	subscription_factory
	subscription_manager
	subscription_type
	task_scheduler
	worker_thread

	model/model
	model/model_a
	model/model_b
	model/model_d

	task/task
	task/task_a
	task/task_b
	task/task_c
	task/task_d
        task/task_image_lim
        task/task_scope_image
        task/task_rescale_tbb
        task/task_norml2tbb
        task/task_gradient_tbb
        task/task_image_img
        task/task_pca_tbb
        task/task_band
        task/task_band2qimage_tbb
        task/task_datarange_tbb
        task/task_normrange_tbb
        task/task_image_norm
        task/task_image_grad
        task/task_image_imgpca

        imagemodel
)

vole_moc_library(
	subscription.h
	subscription_manager.h
	task_scheduler.h
	worker_thread.h

	model/model.h
	model/model_a.h
	model/model_b.h
	model/model_d.h

        imagemodel.h
)

vole_add_module()