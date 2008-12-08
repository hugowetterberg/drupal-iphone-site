<?php
//$Id: date_admin.inc,v 1.40.2.3.2.25 2008/11/30 12:38:36 karens Exp $
/**
 * @file
 * Date administration code.
 * Moved to separate file since there is a lot of code here that is not needed often.
 */
/**
 * Implementation of hook_widget_settings().
 */
function _date_widget_settings($op, &$field) {
  switch ($op) {
    case 'callbacks':
      return array('default value' => CONTENT_CALLBACK_CUSTOM);
    case 'form':
      return date_widget_settings_form($field);

    case 'save':
      return array('default_value', 'default_value_code', 'default_value2', 'default_value_code2', 'input_format', 'input_format_custom', 'increment', 'text_parts', 'year_range', 'label_position');

    case 'validate':
      if ($field['default_value'] == 'strtotime') {
        $is_strtotime = @strtotime($field['default_value_code']);
        if (!$is_strtotime) {
          form_set_error('default_value_code', t('The Strtotime default value is invalid.'));
        }
      }
      if ($field['default_value2'] == 'strtotime') {
        $is_strtotime = @strtotime($field['default_value_code2']);
        if (!$is_strtotime) {
          form_set_error('default_value_code2', t('The Strtotime default value for the To Date is invalid.'));
        }
      }
      if ($field['type'] == 'date_select' && !preg_match('@\-[0-9]*:[\+|\-][0-9]*@', $field['year_range'])) {
        form_set_error('year_range', t('Years back and forward must be in the format -9:+9.'));
      }
      break;
  }
}

/**
 * Custom widget settings manipulation.
 *
 * CCK widget settings can't use form_set_value(),
 * so do it in a custom function.
 */
function date_widget_settings_validate(&$form, &$form_state) {
  if ($form_state['values']['widget_type'] == 'date_popup') {
    // Only a limited set of formats is available for the Date Popup module
    if (!empty($form_state['values']['input_format_custom']) || !in_array($form_state['values']['input_format'], date_popup_formats())) {
      form_set_value($form['input_format_custom'], NULL, $form_state);
      form_set_value($form['input_format'], DATE_FORMAT_DATETIME, $form_state);
    }
    // The jQuery calendar requires year, month, and day or it won't work.
    $date_parts = array('year', 'month', 'day');
    $intersection = array_intersect($date_parts, $form_state['values']['granularity']);
    if ($intersection != $date_parts) {
      form_set_error('granularity', t('The Date Popup widget will not work unless granularity includes year, month, and day.'));
    }
  }
  if (isset($form_state['values']['advanced']['label_position'])) {
    form_set_value($form['label_position'], $form_state['values']['advanced']['label_position'], $form_state);
  }
  // Munge the table display for text parts back into an array of text parts.
  if (is_array($form_state['values']['advanced']['text_parts'])) {
    form_set_value($form['text_parts'], array_keys(array_filter($form_state['values']['advanced']['text_parts'])), $form_state);
  }
}

function date_widget_settings_form($widget) {
  $form = array(
    '#element_validate' => array('date_widget_settings_validate'),
    );
  $form['input']['default_value'] = array(
    '#type' => 'radios', '#title' => t('Default value'),
    '#default_value' => !empty($widget['default_value']) ? $widget['default_value'] : 'blank',
    '#options' => array('blank' => t('Blank'), 'now' => t('Now'), 'strtotime' => t('Relative')),
    '#description' => t("A default value to use for this field. If you select 'Relative', add details below."),
    );
  $form['input']['default'] = array(
    '#type' => 'fieldset',
    '#title' => t('Customize Default Value'),
    '#description' => t("<p>The custom value for a Relative default should be something that describes a time by reference to the current day using strtotime, like '+90 days' (90 days from the day the field is created) or '+1 Saturday' (the next Saturday). See !strtotime for more details.</p>", array('!strtotime' => l(t('strtotime'), 'http://www.php.net/manual/en/function.strtotime.php'))),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    );
  $form['input']['default']['default_value_code'] = array(
    '#type' => 'textfield', '#title' => t('Custom value for From date'),
    '#default_value' => isset($widget['default_value_code']) ? $widget['default_value_code'] : '',
    );

  $form['input']['default']['default_value2'] = array(
    '#type' => 'radios', '#title' => t('Default value for To date'),
    '#default_value' => !empty($widget['default_value2']) ? $widget['default_value2'] : 'same',
    '#options' => array('same' => t('Same as From date'), 'blank' => t('Blank'), 'now' => t('Now'), 'strtotime' => t('Relative')),
    '#description' => t("A default value to use for this field. If you select 'Relative', add details below."),
    );
  $form['input']['default']['default_value_code2'] = array(
    '#type' => 'textfield', '#title' => t('Custom value for To date'),
    '#default_value' => isset($widget['default_value_code2']) ? $widget['default_value_code2'] : '',
    );

  $options = array();
  if ($widget['type'] == 'date_popup' && module_exists('date_popup')) {
    $formats = date_popup_formats();
  }
  else {
    // example input formats must show all possible date parts, so add seconds.
    $formats = str_replace('i', 'i:s', date_short_formats());
    $formats = drupal_map_assoc($formats);
  }
  $now = date_example_date();
  foreach ($formats as $f) {
    $options[$f] = date_format_date($now, 'custom', $f);
  }
  $default_format = array_shift($formats);
  $form['input']['input_format'] = array(
    '#type' => 'select', '#title' => t('Input format'),
    '#default_value' => isset($widget['input_format']) ? $widget['input_format'] : $default_format,
    '#options' => $options,
    '#description' => t('Set the order and format for the date parts in the input form. The format will be adapted to remove values not in the granularity for this field.'),
  );
  // Only a limited set of formats is available for the Date Popup module
  if ($widget['type'] != 'date_popup') {
    $form['input']['format']['input_format_custom'] = array(
      '#type' => 'textfield',  '#title' => t('*Custom input format'),
      '#default_value' => isset($widget['input_format_custom']) ? $widget['input_format_custom'] : '',
      '#description' => t('The custom format, if provided, will override the input format selected above. See more about custom date formats below.'),
    );
  }
  else {
    $form['input']['format']['input_format_custom'] = array(
      '#type' => 'hidden',
      '#value' => '',
      );
  }
  if (in_array($widget['type'], array('date_select', 'date_popup', 'date_select_repeat', 'date_popup_repeat'))) {
    $form['input']['year_range'] = array(
      '#type' => 'textfield',
      '#title' => t('Years back and forward'),
      '#default_value' => !empty($widget['year_range']) ? $widget['year_range'] : '-3:+3',
      '#size' => 10,
      '#maxsize' => 10,
      '#description' => t('Number of years to go back and forward in the year selection list, default is -3:+3.'),
      );
    $form['input']['increment'] = array(
      '#type' => 'select', '#title' => t('Time increment'),
      '#default_value' => isset($widget['increment']) ? $widget['increment'] : 1,
      '#options' => array(1 => 1, 5 => 5, 10 => 10, 15 => 15, 30 => 30),
      '#description' => t('Increment the minute and second fields by this amount.'),
    );
  }
  else {
    $form['increment'] = array(
      '#type' => 'hidden',
      '#value' => !empty($widget['increment']) ? $widget['increment'] : 1,
      );
    $form['year_range'] = array(
      '#type' => 'hidden',
      '#value' => isset($widget['year_range']) ? $widget['year_range'] : '-3:+3',
      );
  }

  $form['label_position'] = array(
    '#type' => 'value',
    '#value' => !empty($widget['label_position']) ? $widget['label_position'] : 'above',
    );
  $form['text_parts'] = array(
    '#type' => 'value',
    '#value' => isset($widget['text_parts']) ? $widget['text_parts'] : '',
    );
  $form['input']['advanced'] = array(
    '#tree' => TRUE,
    '#type' => 'fieldset',
    '#title' => t('Customize Date Parts'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    );
  $form['input']['advanced']['label_position'] = array(
    '#type' => 'radios',
    '#options' => array('above' => t('Above'), 'within' => t('Within'), 'none' => t('None')),
    '#default_value' => !empty($widget['label_position']) ? $widget['label_position'] : 'above',
    '#title' => t('Position of date part labels'),
    '#description' => t("The location of date part labels, like 'Year', 'Month', or 'Day'. 'Above' will display them as titles above each date part. 'Within' will insert the label as the first option in the select list and in blank textfields. 'None' will not label any of the date parts. The exact text in the label is controlled by themes like 'date_part_label_year' and 'date_part_label_month'."),
    );
  $form['input']['advanced']['text_parts'] = array(
    '#theme' => $widget['type'] == 'date_select' ? 'date_text_parts' : '',
    );
  $text_parts = isset($widget['text_parts']) ? (array) $widget['text_parts'] : array();
  foreach (date_granularity_names() as $key => $value) {
    if ($widget['type'] == 'date_select') {
      $form['input']['advanced']['text_parts'][$key] = array(
        '#type' => 'radios',
        '#default_value' => in_array($key, $text_parts) ? 1 : 0,
        '#options' => array(0 => '', 1 => ''),
        );
    }
    else {
      $form['input']['advanced']['text_parts'][$key] = array(
        '#type' => 'hidden',
        '#value' => in_array($key, (array) $widget['text_parts']) ? 1 : 0,
        );
    }
  }
  return $form;
}

/**
 * Display the text/select options for date parts in a table
 * for easier readability.
 */
function theme_date_text_parts($element) {
  $names = date_granularity_names();
  $rows = array();
  foreach ($names as $key => $part) {
    if ($element[$key]['#type'] == 'hidden') {
      $rows[] = drupal_render($element[$key]);
    }
    else {
      $rows[] = array($names[$key], drupal_render($element[$key][0]), drupal_render($element[$key][1]));
    }
  }
  if ($element['year']['#type'] == 'hidden') {
    return implode($rows);
  }
  else {
    $header = array(t('Input Type'), t('Select list'), t('Text field'));
    return theme('table', $header, $rows);
  }
}

/**
 * Implementation of hook_field_settings().
 */
function _date_field_settings($op, $field) {
  switch ($op) {
    case 'form':
      return date_field_settings_form($field);

    case 'validate':
      if (!in_array('year', $field['granularity'])) {
        form_set_error('granularity', t('Granularity must include a year.'));
      }
      if ($field['tz_handling'] != 'none' && !in_array('hour', array_filter($field['granularity']))) {
        form_set_error('tz_handling', t('Dates without hours granularity must not use any timezone handling.'));
      }
      break;

    case 'save':

      $options = array('granularity', 'timezone_db', 'tz_handling', 'todate', 'repeat', 'repeat_collapsed');
      for ($i = 0; $i <= 3; $i++) {
        switch ($i) {
        case(1):
          $name = 'long';
          break;
        case(2):
          $name = 'medium';
          break;
        case(3):
          $name = 'short';
          break;
        default:
          $name = 'default';
        }
        $append = ($i > 0 ? '_'. $name : '');
        $options[] = 'output_format_date'. $append;
        $options[] = 'output_format_custom'. $append;
      }
      return $options;

    case 'database columns':
      return date_columns($field);

    case 'views data':
      $data = content_views_field_views_data($field);
      $db_info = content_database_info($field);
      $table_alias = content_views_tablename($field);
      
      // Swap in the date multiple value handler for 
      // CCK's regular multiple value handler.
      $data[$table_alias][$field['field_name'] .'_value']['field']['handler'] = 'date_handler_field_multiple';

      // Unset the filter and argument handlers, dates can use the generic 
      // date argument and filter handlers created by the Date API.
      unset($data[$table_alias][$field['field_name'] .'_value']['argument']);
      unset($data[$table_alias][$field['field_name'] .'_value']['filter']);
  
      // Add in another set of fields for the To date.
      if (!empty($field['todate'])) {
        $data[$table_alias][$field['field_name'] .'_value2'] = $data[$table_alias][$field['field_name'] .'_value'];
        $data[$table_alias][$field['field_name'] .'_value']['title'] = t('!label (!field value)', array('!label' => t($field['widget']['label']), '!field' => $field['field_name']));
        $data[$table_alias][$field['field_name'] .'_value2']['title'] = t('!label (!field value2)', array('!label' => t($field['widget']['label']), '!field' => $field['field_name']));
      }
      return $data;
  }
}

/**
 *  Callback for field columns.
 */
function date_columns($field) {
  if ($field['type'] == 'date') {
    $db_columns['value'] = array('type' => 'varchar', 'length' => 20, 'not null' => FALSE, 'sortable' => TRUE);
  }
  elseif ($field['type'] == 'datestamp') {
    $db_columns['value'] = array('type' => 'int', 'not null' => FALSE, 'sortable' => TRUE);
  }
  elseif ($field['type'] == 'datetime') {
    $db_columns['value'] = array('type' => 'datetime', 'not null' => FALSE, 'sortable' => TRUE);
  }
  // If a second date is needed for 'To date', just make a copy of the first one.
  if (!empty($field['todate'])) {
    $db_columns['value2'] = $db_columns['value'];
  }
  // timezone and offset columns are used only if date-specific dates are chosen.
  if (isset($field['tz_handling']) && $field['tz_handling'] == 'date') {
    $db_columns['timezone'] = array('type' => 'varchar', 'length' => 50, 'not null' => FALSE, 'sortable' => TRUE);
    $db_columns['offset'] = array('type' => 'int', 'not null' => FALSE, 'sortable' => TRUE);
    if (!empty($field['todate'])) $db_columns['offset2'] = array('type' => 'int', 'not null' => FALSE, 'sortable' => TRUE);
  }
  if (isset($field['repeat']) && $field['repeat'] == 1) {
    $db_columns['rrule'] = array('type' => 'text', 'not null' => FALSE, 'sortable' => FALSE);
  }
  return $db_columns;
}

function date_field_settings_form($field) {
  $form = array(
    '#element_validate' => array('date_field_settings_validate'),
  );
  // Make sure granularity is in the right format and has no empty values.
  if (!empty($field['granularity']) && is_array($field['granularity'])) {
    $granularity = array_filter($field['granularity']);  
  }
  else {
    $granularity = array('year', 'month', 'day', 'hour', 'minute');
  }
  $tz_handling = isset($field['tz_handling']) ? $field['tz_handling'] : (date_has_time($granularity) ? 'site' : 'none');
  
  // If adding a repeat, override the Content module's handling of the multiple values option.
  if (module_exists('date_repeat') && date_is_repeat_field($field)) {
    $form['multiple'] = array('#type' => 'hidden', '#value' => 1);
    $form['repeat'] = array('#type' => 'hidden', '#value' => 1);
  }
  else {
    $form['repeat'] = array('#type' => 'hidden', '#value' => 0);
  }

  $description = t("Display a matching second date field as a 'To date'. If marked 'Optional' field will be presented but not required. If marked 'Required' the 'To date' will be required if the 'From date' is required or filled in.");
  $description .= date_data_loss_warning('To date');
  $form['input']['todate'] = array(
    '#type' => 'radios', '#title' => t('To Date'),
    '#options' => array('' => t('Never'), 'optional' => t('Optional'), 'required' => t('Required')),
    '#description' => $description,
    '#default_value' => isset($field['todate']) ? $field['todate'] : '',
    );
  $form['input']['granularity'] = array(
    '#type' => 'select', '#title' => t('Granularity'),
    '#default_value' => $granularity,
    '#options' => date_granularity_names(),
    '#multiple' => TRUE,
    '#description' => t('Set the date elements to be stored (at least a year is required).'),
    );
  $form['output']['simple'] = date_formatter_setup_form($field, 0);
  $form['output']['simple']['#title'] = t('Default Display');
  $form['output']['simple']['#description'] = theme('advanced_help_topic', 'date_api', 'date-display') . t('Set up formatting for the date display.');

  $form['output']['advanced'] = array(
    '#type' => 'fieldset', '#title' => t('Additional Display Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('Options other than the default are made available to views and themes. Possible formats are default, long, medium, and short.'),
  );
  for ($i = 1; $i <= 3; $i++) {
    $form['output']['advanced'][$i] = date_formatter_setup_form($field, $i);
  }
  $description = t('Select the timezone handling method to be used for this date field.');
  $description .= date_data_loss_warning('Time zone handling');
  $form['tz_handling'] = array(
    '#type' => 'select',
    '#title' => t('Time zone handling'),
    '#default_value' => $tz_handling,
    '#options' => date_timezone_handling_options(),
    '#description' => $description,
  );
  // Force this value to hidden because we don't want to allow it to be changed right now,
  // but allow it to be a variable if needed.
  $form['timezone_db'] = array(
    '#type' => 'hidden',
    '#value' => date_get_timezone_db($tz_handling),
    );
  
  if (module_exists('date_repeat') && date_is_repeat_field($field)) {
    $form['repeat_collapsed'] = array(
      '#type' => 'radios',
      '#default_value' => !empty($field['repeat_collapsed']) ? intval($field['repeat_collapsed']) : 0,
      '#options' => array(0 => t('Expanded'), 1 => t('Collapsed')),
      '#title' => t('Repeat display'),
      '#description' => t("Should the repeat options form start out expanded or collapsed? Set to 'Collapsed' to make those options less obtrusive."),
      );
  }

  $form['#suffix'] = '<div class="form-item"><div class="description">'. t("* The custom format, if provided, will override the selected display or input options. Define a php date format string like 'm-d-Y H:i' (see !link for more details).", array('!link' => l('http://php.net/date', 'http://php.net/date'))) .'</div></div>';

  return $form;
}

/**
 * Custom field settings manipulation.
 *
 * CCK field settings can't use form_set_value(),
 * so do it in a custom function.
 */
function date_field_settings_validate(&$form, &$form_state) {
  if ($form_state['values']['tz_handling'] == 'none') {
    form_set_value($form['timezone_db'], '', $form_state);
  }
  else {
    form_set_value($form['timezone_db'], date_get_timezone_db($form_state['values']['tz_handling']), $form_state);
  }
}

function date_data_loss_warning($name) {
  return t('<p class="error">Changing the %name setting after data has been created could result in the loss of data!</p>', array('%name' => $name));
}

/**
 *  A form to create a date formatter option
 */
function date_formatter_setup_form($field, $delta) {

  switch ($delta) {
  case(1):
    $name = 'long';
    $label = t('Long');
    $default = variable_get('date_format_long',  'l, F j, Y - H:i');
    break;
  case(2):
    $name = 'medium';
    $label = t('Medium');
    $default = variable_get('date_format_medium',  'D, m/d/Y - H:i');
    break;
  case(3):
    $name = 'short';
    $label = t('Short');
    $default = variable_get('date_format_short', 'm/d/Y - H:i');
    break;
  default:
    $name = 'default';
    $label = t('Default');
    $default = variable_get('date_format_short', 'm/d/Y - H:i');
  }
  $append = ($delta > 0 ? '_'. $name : '');
  $form = array(
    '#type' => 'fieldset', '#title' => $label,
    );

  $form['output_format_date'. $append] = array(
    '#type' => 'select', '#title' => t('Date display'),
    '#default_value' => isset($field['output_format_date'. $append]) ? $field['output_format_date'. $append] : $default,
    '#options' => date_format_options(),
    '#multiple' => false,
  );
  $form['output_format_custom'. $append] = array(
    '#type' => 'textfield', '#title' => t('*Custom display format'),
    '#default_value' => isset($field['output_format_custom'. $append]) ? $field['output_format_custom'. $append] : '',
  );
  return $form;
}

/**
 *  Timezone handling options
 *
 *  the 'none' option will do no timezone conversions and will store and display dates exactly as entered
 *  useful in locales or situations where timezone conversions are not working reliably,
 *  for dates with no times, for historical dates where timezones are irrelevant,
 *  or anytime conversion is unnecessary or undesirable
 */
function date_timezone_handling_options() {
  return array(
    'site' => t("Site's time zone"),
    'date' => t("Date's time zone"),
    'user' => t("User's time zone"),
    'utc' => 'UTC',
    'none' => t('No time zone conversion'),
    );
}

/**
 * Get an example date and make sure the difference between
 * month and day and 12 and 24 hours will be clear.
 */
function date_example_date() {
  $now = date_now();
  if (date_format($now, 'm') == date_format($now, 'd')) {
    date_modify($now, '+1 day');
  }
  if (date_format($now, 'H') == date_format($now, 'h')) {
    date_modify($now, '+12 hours');
  }
  return $now;
}

/**
 * Helper function to create a field array with default values for a date 
 * $field, based on field type, widget type, and timezone handling.
 * 
 * This function can be used to make it easier to create new date fields
 * in profiles or custom code.
 * 
 * @param $field_type
 *   Can be 'date', 'datestamp', or 'datetime'.
 * @param $widget_type
 *   Can be 'date_text', 'date_text_repeat', 'date_select', 
 *   'date_select_repeat', 'date_popup', 'date_popup_repeat'.
 * @param $tz_handling
 *   Can be 'site', 'date', 'utc', 'none', or 'user'.
 * @param $overrides
 *   An optional array of field/widget values that should 
 *   override the normal defaults.
 */
function date_field_default_values($field_type, $widget_type, $tz_handling, $overrides = array()) {
  module_load_include('inc', 'content', 'includes/content.crud');
  $repeat_widgets = array('date_select_repeat', 'date_text_repeat', 'date_popup_repeat');
  
  $base_field_values = array (
    'field_name' => '',
    'type_name' => '',
    'multiple' => in_array($widget_type, $repeat_widgets) ? 1 : 0,
    'module' => 'date',
    'granularity' => array (
      'year' => 'year',
      'month' => 'month',
      'day' => 'day',
      'hour' => 'hour',
      'minute' => 'minute',
      ),
    'timezone_db' => date_get_timezone_db($tz_handling),
    'tz_handling' => $tz_handling,
    'todate' => 'optional',
    'repeat' => in_array($widget_type, $repeat_widgets) ? 1 : 0,
    'repeat_collapsed' => 0, 
    'output_format_date' => variable_get('date_format_short', 'm/d/Y - H:i'),
    'output_format_custom' => '', 
    'output_format_date_long' => variable_get('date_format_long',  'l, F j, Y - H:i'),
    'output_format_custom_long' => '', 
    'output_format_date_medium' => variable_get('date_format_medium',  'D, m/d/Y - H:i'),
    'output_format_custom_medium' => '', 
    'output_format_date_short' => variable_get('date_format_short', 'm/d/Y - H:i'),
    'output_format_custom_short' => '',
    );
  $base_widget_values = array (
    'label' => '',
    'default_value' => 'now',
    'default_value_code' => '', 
    'default_value2' => 'same',
    'default_value_code2' => '', 
    'input_format' => variable_get('date_format_short', 'm/d/Y - H:i'),
    'input_format_custom' => '', 
    'increment' => 1,
    'text_parts' => array (),
    'year_range' => '-3:+3',
    'label_position' => 'above',
    'weight' => 0,
    'description' => '',
    'module' => 'date',
  );
  $field_values['date'] = $base_field_values;
  $field_values['date']['type'] = 'date';
  
  $field_values['datestamp'] = $base_field_values;
  $field_values['datestamp']['type'] = 'datestamp';

  $field_values['datetime'] = $base_field_values;
  $field_values['datetime']['type'] = 'datetime';

  $widget_values['date_select'] = $base_widget_values;
  $widget_values['date_select']['type'] = 'date_select';
  
  $widget_values['date_text'] = $base_widget_values;
  $widget_values['date_text']['type'] = 'date_text';
  
  $widget_values['date_popup'] = $base_widget_values;
  $widget_values['date_popup']['type'] = 'date_popup';
  $widget_values['date_popup']['module'] = 'date_popup';
    
  $widget_values['date_select_repeat'] = $base_widget_values;
  $widget_values['date_select_repeat']['type'] = 'date_select_repeat';
  
  $widget_values['date_text_repeat'] = $base_widget_values;
  $widget_values['date_text_repeat']['type'] = 'date_text_repeat';
  
  $widget_values['date_popup_repeat'] = $base_widget_values;
  $widget_values['date_popup_repeat']['type'] = 'date_popup_repeat';
  
  // Get the basic field array with content module default values.
  $field = content_field_default_values($field_type);
  
  // Update it with default values for this date field and widget type.
  foreach ($field_values[$field_type] as $key => $value) {
    $field[$key] = $value;
  }
  foreach ($widget_values[$widget_type] as $key => $value) {
    $field['widget'][$key] = $value;
  }
  
  // Allow overrides of specific default values. 
  foreach ($overrides as $key => $value) {
    if ($key != 'widget') {
      $field[$key] = $value;
    }
    else {
      foreach ($value as $widget_key => $widget_value) {
        $field['widget'][$widget_key] = $widget_value;
      }
    }
  }
  
  // Reset columns and db_storage, which may need to be 
  // adjusted to the new values.
  $field['columns'] = date_columns($field);
  $field['db_storage'] = content_storage_type($field);
  
  return $field;
}