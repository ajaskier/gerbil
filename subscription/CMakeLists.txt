vole_module_name("subscription")
vole_module_description("Subscription Manager && Task Scheduler functionality")
vole_module_variable("Gerbil_Subscription")

vole_add_required_dependencies("BOOST" "TBB" "OPENCV" "QT" "Threads")
vole_add_required_modules("gerbil_gui" "dynamic_any")

vole_compile_library(
	data
	data_entry
	dependency
	lock
	subscription
	subscription_factory
	subscription_manager
	subscription_type
	task_scheduler
	worker_thread
        data_condition_informer
        source_declaration

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
        task/task_image_pca

        task/task_roi
        task/dist/task_distviewbins_tbb
        task/dist/task_dist_sub
        task/dist/task_dist_add
        task/dist/task_dist_sub_arg
        task/dist/task_dist_add_arg

        task/labels/task_labels_icons
        task/labels/task_set_labels
        task/labels/task_labels_alter_pixels
        task/labels/task_add_label
        task/labels/task_merge_labels

        model/img_model
        model/dist_model
        model/labels_model

        distviewcompute_utils

)

vole_moc_library(
	subscription.h
	subscription_manager.h
	task_scheduler.h
	worker_thread.h

        task/task.h
	model/model.h
	model/model_a.h
	model/model_b.h
	model/model_d.h

        model/img_model.h
        model/dist_model.h
        model/labels_model.h
)

vole_add_module()
