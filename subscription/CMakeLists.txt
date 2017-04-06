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
	data_register
	subscription_manager
	subscription_type
	task_scheduler
	worker_thread
    source_declaration

	model/model
	model/model_a
	model/model_b
	model/model_d

	task/task
	task/command_task
	task/tbb_task

	task/task_a
	task/task_b
	task/task_c
	task/task_d
    task/task_image_lim
    task/task_image_rgb_tbb
    task/task_image_bgr_tbb
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

    task/labels/task_labels_icons
    task/labels/task_set_labels
    task/labels/task_labels_alter_pixels
    task/labels/task_add_label
    task/labels/task_merge_labels
    task/labels/task_labels_consolidate

	task/task_meanshift_sp
	task/task_rgb_display

    model/img_model
    model/dist_model
    model/labels_model
	model/clusterization_model
	model/falsecolor_model
	model/falsecolor_task_delegate

    distviewcompute_utils

)

vole_moc_library(
	subscription.h
	subscription_manager.h
	task_scheduler.h
	worker_thread.h

    task/task.h
	task/command_task.h
	task/tbb_task.h

	model/model.h
	model/model_a.h
	model/model_b.h
	model/model_d.h

    model/img_model.h
    model/dist_model.h
    model/labels_model.h
	model/clusterization_model.h
	model/falsecolor_model.h
	model/falsecolor_task_delegate.h

	task/task_meanshift_sp.h
	task/task_rgb_display.h
)

vole_add_module()
