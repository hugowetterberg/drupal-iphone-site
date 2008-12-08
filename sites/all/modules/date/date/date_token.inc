<?php
//$Id: date_token.inc,v 1.2.2.1.2.2 2008/09/01 12:05:12 karens Exp $
/**
 * @file
 * Token module integration.
 */

function date_token_list($type = 'all') {
  if ($type == 'field' || $type == 'all') {
    $tokens = array();

    $tokens['date']['value']          = t("The raw date value.");
    $tokens['date']['view']           = t("The formatted date.");
    $tokens['date']['timestamp']      = t("The raw date timestamp.");
    $tokens['date']['yyyy']           = t("Date year (four digit)");
    $tokens['date']['yy']             = t("Date year (two digit)");
    $tokens['date']['month']          = t("Date month (full word)");
    $tokens['date']['mon']            = t("Date month (abbreviated)");
    $tokens['date']['mm']             = t("Date month (two digit, zero padded)");
    $tokens['date']['m']              = t("Date month (one or two digit)");
    $tokens['date']['ww']             = t("Date week (two digit)");
    $tokens['date']['date']           = t("Date date (day of month)");
    $tokens['date']['day']            = t("Date day (full word)");
    $tokens['date']['ddd']            = t("Date day (abbreviation)");
    $tokens['date']['dd']             = t("Date day (two digit, zero-padded)");
    $tokens['date']['d']              = t("Date day (one or two digit)");

    $tokens['date']['to-????']        = t("If the field has a to-date defined, the same tokens exist in the form: [to-????], where ???? is the normal token.");

    return $tokens;
  }
}

function date_token_values($type, $object = NULL) {
  if ($type == 'field') {
    $item = $object[0];

    $item['value'] = trim($item['value']);
    $item_type = isset($item['date_type']) ? $item['date_type'] : (is_numeric($item['value']) ? DATE_UNIX : DATE_ISO);
    $timezone = !empty($item['timezone']) ? $item['timezone'] : date_default_timezone_name();
    $timezone_db = !empty($item['timezone_db']) ? $item['timezone_db'] : 'UTC';
    $date = date_make_date($item['value2'], $timezone_db, $item_type);
    if ($timezone_db != $timezone) {
      date_timezone_set($date, timezone_open($timezone));
    }

    $tokens['value']          = $item['value'];
    $tokens['view']           = $item['view'];
    $tokens['timestamp']      = date_format_date($date, 'custom', 'U');
    $tokens['yyyy']           = date_format_date($date, 'custom', 'Y');
    $tokens['yy']             = date_format_date($date, 'custom', 'y');
    $tokens['month']          = date_format_date($date, 'custom', 'F');
    $tokens['mon']            = date_format_date($date, 'custom', 'M');
    $tokens['mm']             = date_format_date($date, 'custom', 'm');
    $tokens['m']              = date_format_date($date, 'custom', 'n');
    $tokens['ww']             = date_format_date($date, 'custom', 'W');
    $tokens['date']           = date_format_date($date, 'custom', 'N');
    $tokens['day']            = date_format_date($date, 'custom', 'l');
    $tokens['ddd']            = date_format_date($date, 'custom', 'D');
    $tokens['dd']             = date_format_date($date, 'custom', 'd');
    $tokens['d']              = date_format_date($date, 'custom', 'j');

    if (!empty($item['value2'])) {

      $item['value2'] = trim($item['value2']);
      $date = date_make_date($item['value2'], $timezone_db, $item_type);
      if ($timezone_db != $timezone) {
        date_timezone_set($date, timezone_open($timezone));
      }

      $tokens['to-value']          = $item['value2'];
      $tokens['to-view']           = $item['view2'];
      $tokens['to-timestamp']      = date_format_date($date, 'custom', 'U');
      $tokens['to-yyyy']           = date_format_date($date, 'custom', 'Y');
      $tokens['to-yy']             = date_format_date($date, 'custom', 'y');
      $tokens['to-month']          = date_format_date($date, 'custom', 'F');
      $tokens['to-mon']            = date_format_date($date, 'custom', 'M');
      $tokens['to-mm']             = date_format_date($date, 'custom', 'm');
      $tokens['to-m']              = date_format_date($date, 'custom', 'n');
      $tokens['to-ww']             = date_format_date($date, 'custom', 'W');
      $tokens['to-date']           = date_format_date($date, 'custom', 'N');
      $tokens['to-day']            = date_format_date($date, 'custom', 'l');
      $tokens['to-ddd']            = date_format_date($date, 'custom', 'D');
      $tokens['to-dd']             = date_format_date($date, 'custom', 'd');
      $tokens['to-d']              = date_format_date($date, 'custom', 'j');
    }
    return $tokens;
  }
}
