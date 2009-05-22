<?php
// $Id: securesite.theme.inc,v 1.1.2.1 2009/03/09 14:48:27 darrenoh Exp $

/**
 * @file
 * Theme functions.
 */

/**
 * Process variables for securesite-user-login.tpl.php
 *
 * @param $variables
 *   An array of variables from the theme system.
 */
function template_preprocess_securesite_user_login(&$variables) {
  $variables['title'] = variable_get('securesite_login_form', t('Enter your user name and password.'));
  $variables['messages'] = theme('status_messages');
}

/**
 * Process variables for securesite-user-pass.tpl.php
 *
 * @param $variables
 *   An array of variables from the theme system.
 */
function template_preprocess_securesite_user_pass(&$variables) {
  $variables['title'] = variable_get('securesite_reset_form', t('Enter your user name or e-mail address.'));
  $variables['form']['name']['#required'] = FALSE;
}
