<?php
// $Id: flag.views_default.inc,v 1.1.2.2 2008/10/26 09:39:06 mooffie Exp $

/**
 * @file
 * Provides support for the Views module.
 */

/**
 * Implementation of hook_views_default_views()
 */
function flag_views_default_views() {
  $views = array();
  _flag_views_node_default_views($views);
  _flag_views_user_default_views($views);
  return $views;
}

function _flag_views_node_default_views(&$views) {
  $flags = flag_get_flags('node');

  foreach ($flags as $flag) {
    $menu_title = $flag->global ? drupal_ucfirst($flag->get_title()) : t('My !flag-title', array('!flag-title' => drupal_strtolower($flag->get_title())));

    $view = new view;
    $view->name = 'flag_'. $flag->name;
    $view->description = t('View for flag: !flag-title', array('!flag-title' => $flag->get_title()));
    $view->tag = 'flag';
    $view->view_php = '';
    $view->base_table = 'node';
    $view->is_cacheable = '0';
    $view->api_version = 2;
    $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
    $handler = $view->new_display('default', 'Defaults', 'default');
    $handler->override_option('relationships', array(
      'flag_content_rel' => array(
        'label' => $flag->name,
        'required' => 1,
        'flag' => $flag->name,
        'user_scope' => 'current',
        'id' => 'flag_content_rel',
        'table' => 'node',
        'field' => 'flag_content_rel',
        'relationship' => 'none',
      ),
      'uid' => array(
        'label' => 'user',
        'required' => 0,
        'id' => 'uid',
        'table' => 'flag_content',
        'field' => 'uid',
        'relationship' => 'flag_content_rel',
      ),
    ));

    $fields = array(
      'type' => array(
        'id' => 'type',
        'table' => 'node',
        'field' => 'type',
        'label' => 'Type',
      ),
      'title' => array(
        'id' => 'title',
        'table' => 'node',
        'field' => 'title',
        'label' => 'Title',
        'link_to_node' => 1,
      ),
      'name' => array(
        'label' => 'Author',
        'link_to_user' => 1,
        'id' => 'name',
        'table' => 'users',
        'field' => 'name',
      ),
    );

    if (module_exists('comment')) {
      $fields += array(
        'comment_count' => array(
          'id' => 'comment_count',
          'table' => 'node_comment_statistics',
          'field' => 'comment_count',
          'label' => 'Replies',
        ),
        'last_comment_timestamp' => array(
          'id' => 'last_comment_timestamp',
          'table' => 'node_comment_statistics',
          'field' => 'last_comment_timestamp',
          'label' => 'Last Post',
        ),
      );
    }

    $fields += array(
      'ops' => array(
        'label' => 'Ops',
        'id' => 'ops',
        'table' => 'flag_content',
        'field' => 'ops',
        'relationship' => 'flag_content_rel',
      ),
    );

    $handler->override_option('fields', $fields);
    $handler->override_option('filters', array(
      'status' => array(
        'operator' => '=',
        'value' => 1,
        'group' => '0',
        'exposed' => FALSE,
        'expose' => array(
          'operator' => FALSE,
          'label' => '',
        ),
        'id' => 'status',
        'table' => 'node',
        'field' => 'status',
        'relationship' => 'none',
      ),
    ));
    $handler->override_option('access', array(
      'type' => 'role',
      'role' => drupal_map_assoc($flag->roles),
      'perm' => '',
    ));

    $style_options = array(
      'columns' => array(),
      'default' => module_exists('comment') ? 'last_comment_timestamp' : 'title',
      'info' => array(
        'type' => array(
          'sortable' => TRUE,
        ),
        'title' => array(
          'sortable' => TRUE,
        ),
        'name' => array(
          'sortable' => TRUE,
        ),
      ),
      'override' => FALSE,
      'order' => 'asc',
    );

    if (module_exists('comment')) {
      $style_options['info'] += array(
        'comment_count' => array(
          'sortable' => TRUE,
        ),
        'last_comment_timestamp' => array(
          'sortable' => TRUE,
        ),
      );
    }

    $handler->override_option('title', $menu_title);
    $handler->override_option('items_per_page', '25');
    $handler->override_option('use_pager', TRUE);
    $handler->override_option('style_plugin', 'table');
    $handler->override_option('style_options', $style_options);
    $handler = $view->new_display('page', 'Page', 'page_1');
    $handler->override_option('path', 'flags/'. $flag->name);
    $handler->override_option('menu', array(
      'type' => 'normal',
      'title' => $menu_title,
      'weight' => '0',
    ));
    $handler->override_option('tab_options', array(
      'type' => 'none',
      'title' => NULL,
      'weight' => NULL,
    ));

    $views[$view->name] = $view;
  }
}

function _flag_views_user_default_views(&$views) {
  $flags = flag_get_flags('user');

  foreach ($flags as $flag) {
    $menu_title = $flag->global ? drupal_ucfirst($flag->get_title()) : t('My !flag-title', array('!flag-title' => drupal_strtolower($flag->get_title())));

    $view = new view;
    $view->name = 'flag_' . $flag->name;
    $view->description = t('View for flag: !flag-title', array('!flag-title' => $flag->get_title()));
    $view->tag = 'flag';
    $view->view_php = '';
    $view->base_table = 'users';
    $view->is_cacheable = FALSE;
    $view->api_version = 2;
    $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
    $handler = $view->new_display('default', 'Defaults', 'default');

    $handler->override_option('relationships', array(
      'flag_content_rel' => array(
        'label' => $flag->name,
        'required' => 1,
        'flag' => $flag->name,
        'user_scope' => 'current',
        'id' => 'flag_content_rel',
        'table' => 'users',
        'field' => 'flag_content_rel',
        'relationship' => 'none',
      ),
    ));

    $handler->override_option('fields', array(
      'name' => array(
        'label' => 'Name',
        'link_to_user' => 1,
        'exclude' => 0,
        'id' => 'name',
        'table' => 'users',
        'field' => 'name',
        'relationship' => 'none',
      ),
      'ops' => array(
        'label' => 'Ops',
        'exclude' => 0,
        'id' => 'ops',
        'table' => 'flag_content',
        'field' => 'ops',
        'relationship' => 'flag_content_rel',
      ),
    ));

    $handler->override_option('style_plugin', 'table');
    $handler->override_option('style_options', array(
      'grouping' => '',
      'override' => 1,
      'sticky' => 0,
      'order' => 'asc',
      'columns' => array(
        'name' => 'name',
        'ops' => 'ops',
      ),
      'info' => array(
        'name' => array(
          'sortable' => 1,
          'separator' => '',
        ),
        'ops' => array(
          'separator' => '',
        ),
      ),
      'default' => 'name',
    ));

    $handler->override_option('access', array(
      'type' => 'role',
      'role' => drupal_map_assoc($flag->roles),
      'perm' => '',
    ));

    $handler->override_option('title', $menu_title);
    $handler->override_option('items_per_page', 30);
    $handler->override_option('use_pager', TRUE);
    $handler->override_option('empty', 'No users are flagged.');
    $handler->override_option('empty_format', '1');

    $handler = $view->new_display('page', 'Page', 'page_1');
    $handler->override_option('path', 'flags/' . $flag->name);
    $handler->override_option('menu', array(
      'type' => 'normal',
      'title' => $menu_title,
      'weight' => '0',
    ));
    $handler->override_option('tab_options', array(
      'type' => 'none',
      'title' => NULL,
      'weight' => 0,
    ));
    
    $views[$view->name] = $view;
  }
}
