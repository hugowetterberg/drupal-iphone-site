<?php
// $Id: menu_service.inc,v 1.1.2.2 2008/09/01 12:49:19 brmassa Exp $
/**
 * @file
 * Adds a service type for a menu
 *
 * @note This file has a 80 character width limit for all lines
 */

/**
 * Get the menu from the database.
 *
 * @return
 *   Array. A list of all children menu items from a given menu item.
 */
function menu_service_get($menu_id = NULL, $fields = array()) {
  if ($menu_id === NULL or trim($menu_id) === '') {
    $menu_id = 'primary-links';
  }
  if (sizeof($fields) === 0) {
    $fields = FALSE;
  }
  $data = menu_tree_page_data($menu_id);
  return menu_service_process($data, $fields);
}

/**
 * Recursively returns menu items
 *
 * @param data
 *   Array. The current menu item.
 * @return
 *   Array. All children menu items from a given menu item.
 */
function menu_service_process($data, $fields) {
  $out = array();
  if (is_array($data)) {
    foreach ($data as $item) {
      $tmp = new stdClass;
      if ($fields) {
        foreach ($fields as $field) {
          $tmp->{$field} = $item['link'][$field];
        }
      }
      else {
        $tmp = (object) $item['link'];
      }
      if (!$fields or in_array('path_alias', $fields, TRUE)) {
        $tmp->path_alias = drupal_get_path_alias($tmp->link_path);
      }
      $tmp->children = menu_service_process($item['below'], $fields);
      $out[] = $tmp;
    }
  }
  return $out;
}
