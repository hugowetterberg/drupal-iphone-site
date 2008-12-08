<?php
// $Id: services_admin_keys.inc,v 1.3.2.13 2008/09/06 04:13:07 marcingy Exp $
/**
 * @author Services Dev Team
 * @file
 *  Generate security keys.
 */

function services_admin_keys_list() {
  $keys = services_get_keys();
  $header = array(t('Key'), t('Title'), t('Domain'), array('data' => t('Operations'), 'colspan' => '2'));
  $rows = array();

  foreach ($keys as $kid => $key) {
    $row = array();

    $row[] = $kid;
    $row[] = $key->title;
    $row[] = $key->domain;

    // Populate the operations field.
    $operations = array();

    // Set the edit column.
    $operations[] = array('data' => l(t('edit'), 'admin/build/services/keys/'. $kid));

    // Set the delete column.
    $operations[] = array('data' => l(t('delete'), 'admin/build/services/keys/'. $kid .'/delete'));

    foreach ($operations as $operation) {
      $operation['class'] = $class;
      $row[] = $operation;
    }
    $rows[] = $row;
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No API keys created.'), 'colspan' => '5', 'class' => 'message'));
  }

  return theme('table', $header, $rows);
}

function services_admin_keys_form() {
  $kid = arg(4);

  $key = db_fetch_object(db_query("SELECT * FROM {services_keys} WHERE kid = '%s'", $kid));

  $form['kid'] = array(
    '#type'           => 'hidden',
    '#default_value'  => $key->kid,
  );

  if ($key->kid) {
    $form['key'] = array(
      '#type'           => 'markup',
      '#title'          => t('Key'),
      '#value'          => '<strong>'. t('API Key') .':</strong> '. $key->kid,
    );
  }

  $form['title'] = array(
    '#title'          => t('Application title'),
    '#type'           => 'textfield',
    '#default_value'  => $key->title,
    '#description'    => t('The title of the application or website using the service.'),
  );
  $form['domain'] = array(
    '#title'          => t('Allowed domain'),
    '#type'           => 'textfield',
    '#default_value'  => $key->domain,
    '#description'    => t('External domain allowed to use this key.'),
  );

  $form['submit'] = array(
    '#type'           => 'submit',
    '#value'          => $key->title ? t('Save key') : t('Create key'),
  );

  return $form;
}

function services_admin_keys_form_submit($form, &$form_state) {
  services_admin_keys_save($form_state['values']);

  $form_state['redirect']  =  'admin/build/services/keys';
}

function services_admin_keys_save(&$key) {
  $is_existing = FALSE;
  $key['kid'] = !empty($key['kid']) ? $key['kid'] : md5(uniqid(mt_rand(), TRUE));
  $is_existing =  db_result(db_query("SELECT count(*) FROM {services_keys}
    WHERE kid = '%s'", $key['kid']));

  if ($is_existing) {
    db_query("UPDATE {services_keys} SET title = '%s', domain = '%s'
      WHERE kid = '%s'", $key['title'], $key['domain'], $key['kid']);
    return SAVED_UPDATED;
  }
  else {
    db_query("INSERT INTO {services_keys} (kid, title, domain)
      VALUES ('%s', '%s', '%s')", $key['kid'], $key['title'], $key['domain']);
    return SAVED_NEW;
  }
}

function services_admin_keys_delete($kid) {
  db_query("DELETE FROM {services_keys} WHERE kid = '%s'", $kid);
}

function services_admin_keys_delete_confirm(&$form_state, $kid = 0) {
  $key = db_fetch_object(db_query("SELECT * FROM {services_keys} WHERE kid = '%s'", $kid));

  $form['kid'] = array('#type' => 'value', '#value' => $key->kid);

  $message = t('Are you sure you want to delete the key %key?', array('%key' => $key->kid));
  $caption = '';

  $caption .= '<p>'. t('This action cannot be undone.') .'</p>';

  return confirm_form($form, $message, 'admin/build/services/keys', $caption, t('Delete'));
}

function services_admin_keys_delete_confirm_submit($form, &$form_state) {
  services_admin_keys_delete($form_state['values']['kid']);

  $t_args = array('%key' => $form_state['kid']);
  drupal_set_message(t('The key %key has been deleted.', $t_args));
  watchdog('menu', 'Deleted key %key.', array('%key' => $t_args), WATCHDOG_NOTICE);

  $form_state['redirect'] = 'admin/build/services/keys';
}
