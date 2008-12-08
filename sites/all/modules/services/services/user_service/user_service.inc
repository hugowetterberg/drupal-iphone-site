<?php
// $Id: user_service.inc,v 1.1.2.4 2008/09/01 14:32:51 brmassa Exp $
/**
 * @author Services Dev Team
 * @file
 *  Link general user functionalities to services module.
 */

/**
 * Delete an user.
 *
 * @param $uid
 *   Number. The user ID.
 */
function user_service_delete($uid) {
  $account = user_load($uid);
  if (empty($account)) {
    return services_error(t('There is no user with such ID.'));
  }
  user_delete($account, $uid);

  // Everything went right.
  return TRUE;
}

/**
 * Check if the user is allowed to delete the user.
 *
 * @param $uid
 *   Number. The user ID.
 */
function user_service_delete_access($uid) {
  global $user;
  return (($user->uid == $uid and user_access('delete own user')) or
    ($user->uid != $uid and user_access('delete any user')));
}

/**
 * Get user details.
 *
 * @param $uid
 *   Number. The user ID.
 */
function user_service_get($uid) {
  $account = user_load($uid);
  if (empty($account)) {
    return services_error(t('There is no user with such ID.'));
  }

  // Everything went right.
  return $account;
}

/**
 * Check if the user is allowed to get the user data.
 *
 * @param $uid
 *   Number. The user ID.
 */
function user_service_get_access($uid) {
  global $user;
  return (($user->uid == $uid and user_access('get own user data')) or
    ($user->uid != $uid and user_access('get any user data')));
}

/**
 * Login a user
 *
 * @param $username
 *   String. The username.
 * @param $password
 *   String. The user password.
 */
function user_service_login($username, $password) {
  global $user;

  if ($user->uid) {
    // user is already logged in
    return services_error(t('Already logged in as !user.', array('!user' => $user->name)));
  }

  $user = user_authenticate(array('name' => $username, 'pass' => $password));

  if ($user->uid) {
    session_start();
    $return = new stdClass();
    $return->sessid = session_id();
    $return->user = $user;
    return $return;
  }
  session_destroy();
  return services_error(t('Wrong username or password.'));
}

/**
 * Logout user
 */
function user_service_logout() {
  global $user;

  if (!$user->uid) {
    // User is not logged in
    return services_error(t('User is not logged in.'));
  }

  watchdog('user', 'Session closed for %name.', array('%name' => theme('placeholder', $user->name)));

  // Destroy the current session:
  session_destroy();
  module_invoke_all('user', 'logout', NULL, $user);

  // Load the anonymous user
  $user = drupal_anonymous_user();

  return TRUE;
}

/**
 * Save user details.
 *
 * @param $user_data
 *   Object. The user object with all user data.
 */
function user_service_save($account) {
  if (!$account = user_save($account)) {
    return services_error(t('Error on saving the user.'));
  }

  // Everything went right.
  // Return the user ID
  return $account->uid;
}

/**
 * Check if the user is allowed to get the user data.
 *
 * @param $uid
 *   Number. The user ID.
 */
function user_service_save_access($account) {
  global $user;
  return ((empty($account->uid) and user_access('create new users')) or
    ($user->uid == $account->uid and user_access('update own user data')) or
    ($user->uid !=  $account->uid and user_access('update any user data')));
}
