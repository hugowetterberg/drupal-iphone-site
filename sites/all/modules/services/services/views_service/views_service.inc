<?php
// $Id: views_service.inc,v 1.1.2.9 2008/09/01 19:40:15 brmassa Exp $
/**
 * @author Services Dev Team
 * @file
 *  Link general views functionalities to services module.
 */

/**
 * Get a view from the database.
 *
 * @param $view_name
 *   String. The views name.
 * @param $display_id
 *   String (optional).
 * @param $args
 *   Array (optional). A list of params.
 * @return
 *   Array. The views return.
 */
function views_service_get($view_name, $fields = array(), $args = array(), $offset = 0, $limit = 0) {
  $view = views_get_view($view_name);

  // Put all arguments and then execute
  $view->set_arguments($args, FALSE);
  $view->set_offset($offset);
  $view->set_items_per_page($limit);
  $view->execute();

  return $view->result;
}

/**
 * Check the access permission to a given views.
 *
 * @param view_name
 *   String. The views name.
 * @return
 *   Boolean. TRUE if the user is allowed to load the given view.
 */
function views_service_get_access($view_name) {
  $view = views_get_view($view_name);
  if (empty($view)) {
    return FALSE;
  }

  global $user;
  return views_access($view);
}

/**
 * Export a view.
 *
 * @param view_name
 *   String. The views name.
 * @return
 *   Array. The view object.
 */
function views_service_export($view_name) {
  $view = views_get_view($view_name);
  if (empty($view)) {
    return services_error('View does not exist.');
  }

  return $view->export();
}

/**
 * Import a view.
 *
 * @param $view_import
 *   Array. The view object.
 * @param $view_name
 *   String (optional). The view name.
 * @return
 *   Number. The new view ID
 */
function views_service_import($view_import, $view_name = '') {
  // Include the necessary files
  require_once drupal_get_path('module', 'views') .'/includes/admin.inc';

  // Import the the views using the same form as in-site import
  $form_state['values']['name'] = $view_name;
  $form_state['values']['view'] = $view_import;
  $form_state['values']['op'] = t('Import');
  drupal_execute('views_ui_import_page', $form_state);

  // Check if there is a any error
  if ($errors = form_set_error()) {
    return services_error($errors);
  }

  // At this point, the new view was only cached and now its time
  // to save it and return the new View ID
  $view = $form_state['view'];
  $view->save();
  return $view->vid;
}
