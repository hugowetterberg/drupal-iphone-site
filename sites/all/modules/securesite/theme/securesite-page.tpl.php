<?php
// $Id: securesite-page.tpl.php,v 1.1.2.1 2009/03/09 14:48:27 darrenoh Exp $

/**
 * @file
 * Template for Secure Site pages.
 *
 * @see template_preprocess_page()
 */
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">

<head>
<title><?php print $head_title ?></title>
<?php print $head ?>
<link type="text/css" rel="stylesheet" media="all" href="<?php print $base_path . drupal_get_path('module', 'securesite') .'/theme/securesite.css' ?>" />
<?php print $scripts ?>
</head>
<body>
<?php print $messages ?>
<?php print $content ?>
</body>
</html>
