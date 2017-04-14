vole_module_name("test")
vole_module_description("Gerbil tests")
vole_module_variable("Gerbil_Test")

vole_add_required_dependencies("BOOST" "OPENCV" "QT")
vole_add_required_modules("subscription")

vole_compile_library(
  task_scheduler_mock
  data_task_chain

  test_task_core
  test_multi_data_subscriptions
  test_random_data_subscription

  test_image
  test_image_img
  test_image_norm
  test_image_grad
  test_image_imgpca
  test_image_gradpca
  test_image_rgb
  test_image_bgr

  test_labels
  test_labels_icons

  test_dist_img
  test_dist_img_roi_update
  test_dist_img_image_img_update

)

vole_moc_library(
  task_scheduler_mock.h

  test_task_core.h
  test_multi_data_subscriptions.h
  test_random_data_subscription.h

  test_image.h
  test_image_img.h
  test_image_norm.h
  test_image_grad.h
  test_image_imgpca.h
  test_image_gradpca.h
  test_image_rgb.h
  test_image_bgr.h

  test_labels.h
  test_labels_icons.h

  test_dist_img.h
  test_dist_img_roi_update.h
  test_dist_img_image_img_update.h


)

vole_add_test("test_random_data_subscription" "test_random_data_subscription")
vole_add_test("test_multi_data_subscriptions" "test_multi_data_subscriptions")
vole_add_test("test_image" "test_image")
vole_add_test("test_image_img" "test_image_img")
vole_add_test("test_image_norm" "test_image_norm")
vole_add_test("test_image_grad" "test_image_grad")
vole_add_test("test_image_imgpca" "test_image_imgpca")
vole_add_test("test_image_gradpca" "test_image_gradpca")

vole_add_test("test_image_rgb" "test_image_rgb")
vole_add_test("test_image_bgr" "test_image_bgr")

vole_add_test("test_labels" "test_labels")
vole_add_test("test_labels_icons" "test_labels_icons")

vole_add_test("test_dist_img" "test_dist_img")
vole_add_test("test_dist_img_roi_update" "test_dist_img_roi_update")
vole_add_test("test_dist_img_image_img_update" "test_dist_img_image_img_update")

vole_add_module()
