<?php
// $Id: securesite-user-login.tpl.php,v 1.1.2.1 2009/03/09 14:48:27 darrenoh Exp $

/**
 * @file
 * Template for Secure Site log-in form.
 *
 * @see template_preprocess_securesite_user_login()
 */
?>
<h1><?php print t('Log in') ?></h1>
<?php print $messages ?>
<p><?php print $title ?></p>
<?php print drupal_render($form['openid_identifier']); ?>
<?php print drupal_render($form['name']); ?>
<?php print drupal_render($form['pass']); ?>
<?php print drupal_render($form['submit']); ?>
<?php print drupal_render($form['openid_links']); ?>
<?php print drupal_render($form); ?>
<span></span>
