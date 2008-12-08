<?php
// $Id: date_repeat_form.inc,v 1.15.4.8 2008/11/09 12:01:51 karens Exp $
/**
 * @file
 * Code to add a date repeat selection form to a date field and create
 * an iCal RRULE from the chosen selections.
 *
 * Moved to a separate file since it is not used on most pages
 * so the code is not parsed unless needed.
 *
 * Currently implemented:
 * INTERVAL, UNTIL, EXDATE, BYDAY, BYMONTHDAY, BYMONTH,
 * YEARLY, MONTHLY, WEEKLY, DAILY
 *
 * Currently not implemented:
 *
 * BYYEARDAY, MINUTELY, HOURLY, SECONDLY, BYMINUTE, BYHOUR, BYSECOND
 *   These could be implemented in the future.
 *
 * COUNT
 *   The goal of this module is to create a way we can parse an iCal
 *   RRULE and pull out just dates for a specified date range, for
 *   instance with a date that repeats daily for several years, we might
 *   want to only be able to pull out the dates for the current year.
 *
 *   Adding COUNT to the rules we create makes it impossible to do that
 *   without parsing and computing the whole range of dates that the rule
 *   will create. COUNT is left off of the user form completely for this
 *   reason.
 *
 * BYSETPOS
 *   Seldom used anywhere, so no reason to complicated the code.
 */
/**
 * Generate the repeat setting form.
 */
function _date_repeat_rrule_process($element, $edit, $form_state, $form) {
  require_once('./'. drupal_get_path('module', 'date_api') .'/date_api_ical.inc');
  
  if (empty($element['#date_repeat_widget'])) {
    $element['#date_repeat_widget'] = module_exists('date_popup') ? 'date_popup' : 'date_select';
  }
  if (is_array($element['#value'])) {
    $element['#value'] = date_repeat_merge($element['#value'], $element);
    $rrule = date_api_ical_build_rrule($element['#value']);
  }
  else {
    $rrule = $element['#value'];
  }
  // Empty the original string value of the RRULE so we can create
  // an array of values for the form from the RRULE's contents.
  $element['#value'] = '';

  $parts = date_repeat_split_rrule($rrule);
  $rrule = $parts[0];
  $exceptions = $parts[1];
  $timezone = !empty($element['#date_timezone']) ? $element['#date_timezone'] : date_default_timezone_name();
  $merged_values = date_repeat_merge($rrule, $element);
  
  $UNTIL = '';
  if (!empty($merged_values['UNTIL']['datetime'])) {
    $until_date = date_make_date($merged_values['UNTIL']['datetime'], $merged_values['UNTIL']['tz']);
    date_timezone_set($until_date, timezone_open($timezone));
    $UNTIL = date_format($until_date, DATE_FORMAT_DATETIME);
  }

  $element['#type'] = 'fieldset';
  $element['#title'] = t('Repeat');
  $element['#description'] = t('Choose a frequency and period to repeat this date. If nothing is selected, the date will not repeat.');
  $element['#collapsible'] = TRUE;
  $element['#collapsed'] = !empty($element['#date_repeat_collapsed']) ? $element['#date_repeat_collapsed'] : 0;
  
  // Make sure we don't get floating parts where we don't want them.
  $element['#prefix'] = '<div class="date-clear">';
  $element['#suffix'] = '</div>';
  
  $element['INTERVAL'] = array(
    '#type' => 'select',
    //'#title' => t('Interval'),
    '#default_value' => (!empty($rrule['INTERVAL']) ? $rrule['INTERVAL'] : 0),
    '#options' => INTERVAL_options(),
    '#prefix' => '<div class="date-repeat-input">',
    '#suffix' => '</div>',
  );

  $element['FREQ'] = array(
    '#type' => 'select',
    //'#title' => t('Frequency'),
    '#default_value' => !empty($rrule['FREQ']) ? $rrule['FREQ'] : 'NONE',
    '#options' => FREQ_options(),
    '#prefix' => '<div class="date-repeat-input">',
    '#suffix' => '</div>',
  );

  $element['UNTIL'] = array(
    '#tree' => TRUE,
    '#prefix' => '<div class="date-clear">',
    '#suffix' => '</div>',
    'datetime' => array(
      '#type' => $element['#date_repeat_widget'],
      '#title' => t('Until'),
      '#description' => t('Date to stop repeating this item.'),
      '#default_value' => $UNTIL,
      '#date_timezone' => $timezone,
      '#date_format' => !empty($element['#date_format']) ? $element['#date_format'] : 'Y-m-d',
      '#date_text_parts'  => !empty($element['#date_text_parts']) ? $element['#date_text_parts'] : array(),
      '#date_year_range'  => !empty($element['#date_year_range']) ? $element['#date_year_range'] : '-3:+3',
      '#date_label_position' => !empty($element['#date_label_position']) ? $element['#date_label_position'] : 'within',
      ),
    'tz' => array('#type' => 'hidden', '#value' => $element['#date_timezone']),
    'all_day' => array('#type' => 'hidden', '#value' => 1),
    'granularity' => array('#type' => 'hidden', '#value' => serialize(array('year', 'month', 'day'))),
    );
  
  $collapsed = TRUE;
  if (!empty($merged_values['BYDAY']) || !empty($merged_values['BYMONTH'])) {
    $collapsed = FALSE;
  }
	// start the advanced fieldset
  $element['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced'),
    '#collapsible' => TRUE,
    '#collapsed' => $collapsed,
    '#description' => t("If no advanced options are selected, the date will repeat on the day of week of the start date for weekly repeats, otherwise on the month and day of the start date. Use the options below to override that behavior to select specific months and days to repeat on. Use the 'Except' box to input dates that should be omitted from the results.") .' ',
    '#prefix' => '<div class="date-clear">',
    '#suffix' => '</div>',
    );

  $element['advanced']['BYMONTH'] = array(
    '#type' => 'select',
    '#title' => t('Month'),
    '#default_value' => !empty($rrule['BYMONTH']) ? $rrule['BYMONTH'] : '',
    '#options' => array('' => t('-- Any')) + date_month_names(TRUE),
    '#multiple' => TRUE,
    '#size' => 10,
    '#prefix' => '<div class="date-repeat-input">',
    '#suffix' => '</div>',
  );

  $element['advanced']['BYMONTHDAY'] = array(
    '#type' => 'select',
    '#title' => t('Day of Month'),
    '#default_value' => !empty($rrule['BYMONTHDAY']) ? $rrule['BYMONTHDAY'] : '',
    '#options' => array('' => t('-- Any')) + drupal_map_assoc(range(1, 31)) + drupal_map_assoc(range(-1, -31)),
    '#multiple' => TRUE,
    '#size' => 10,
    '#prefix' => '<div class="date-repeat-input">',
    '#suffix' => '</div>',
  );

  $element['advanced']['BYDAY'] = array(
    '#type' => 'select',
    '#title' => t('Day of Week'),
    '#default_value' => !empty($rrule['BYDAY']) ? $rrule['BYDAY'] : '',
    '#options' => array('' => t('-- Any')) + date_repeat_dow_options(),
    //'#attributes' => array('size' => '5'),
    '#multiple' => TRUE,
    '#size' => 10,
    '#prefix' => '<div class="date-repeat-input">',
    '#suffix' => '</div>',
  );

  $element['exceptions'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapased' => empty($exceptions) ? TRUE : FALSE,
    '#title' => t('Except'),
    '#description' => t('Dates to omit from the list of repeating dates.'),
    '#prefix' => '<div class="date-repeat">',
    '#suffix' => '</div>',
    );
  $max = !empty($exceptions) ? sizeof($exceptions) : 0;
  for ($i = 0; $i <= $max; $i++) {
    $EXCEPT = '';
    if (!empty($exceptions[$i]['datetime'])) {
      $EXCEPT = $exceptions[$i]['datetime'];
    }
    $element['exceptions']['EXDATE'][$i] = array(
      '#tree' => TRUE,
      'datetime' => array(
        '#type' => $element['#date_repeat_widget'],
        '#default_value' => $EXCEPT,
        '#date_timezone' => !empty($element['#date_timezone']) ? $element['#date_timezone'] : date_default_timezone_name(),
        '#date_format' => !empty($element['#date_format']) ? $element['#date_format'] : 'Y-m-d',
        '#date_text_parts'  => !empty($element['#date_text_parts']) ? $element['#date_text_parts'] : array(),
        '#date_year_range'  => !empty($element['#date_year_range']) ? $element['#date_year_range'] : '-3:+3',
        '#date_label_position' => !empty($element['#date_label_position']) ? $element['#date_label_position'] : 'within',
        ),
      'tz' => array('#type' => 'hidden', '#value' => $element['#date_timezone']),
      'all_day' => array('#type' => 'hidden', '#value' => 1),
      'granularity' => array('#type' => 'hidden', '#value' => serialize(array('year', 'month', 'day'))),
      );
  }
  $element['help'] = array(
    '#type' => 'fieldset',
    '#title' => t('Help'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t("<p>Dates will match any selection within the same box [January OR June]. When more than one box has criteria selected, all of them are combined to create repeats [[January OR June] AND [Day 1 OR Day 15]]. Positive numbers count from the beginning of the period. Negative numbers count from the end of the period, i.e. -1 is the last, -2 is the next to last, etc.</p><ul><li>If you select 'Every Year' above, and 'March' from 'Month' and '1' and '15' from 'Day of Month' in the Advanced options you will be selecting the 1st and 15th days of March of every year.</li><li>If you select 'Every other Month' above, and 'Second Tuesday' in the Advanced options you will be selecting the second Tuesday of every other month.</li><li>If you select 'Every Year' above, and 'Second Tuesday' in the Advanced options you will be selecting the second Tuesday of every year.</li><li>If you select 'Every Month' above, and 'January' and 'June' and 'First Saturday' in the Advanced options, you will be selecting the first Saturday in January or June.</li><li>If you select 'Every Month' above, and '-1' from 'Day of Month' in the Advanced options you will be selecting the last day of every month.</li></ul>"),
    );
  return $element;
}

/**
 * Regroup values back into a consistant array, no matter what state it is in.
 */
function date_repeat_merge($form_values, $element) {
  if (empty($form_values) || !is_array($form_values)) {
    return $form_values;
  }
  if (array_key_exists('advanced', $form_values) || array_key_exists('exceptions', $form_values)) {
    if (!array_key_exists('advanced', $form_values)) $form_values['advanced'] = array();
    if (!array_key_exists('exceptions', $form_values)) $form_values['exceptions'] = array();
    if (!array_key_exists('help', $form_values)) $form_values['help'] = array();
    $form_values = array_merge($form_values, (array) $form_values['advanced'], (array) $form_values['exceptions']);
    unset($form_values['advanced']);
    unset($form_values['exceptions']);
    unset($form_values['help']);
  }
  if (array_key_exists('BYDAY', $form_values)) unset($form_values['BYDAY']['']);
  if (array_key_exists('BYMONTH', $form_values)) unset($form_values['BYMONTH']['']);
  if (array_key_exists('BYMONTHDAY', $form_values)) unset($form_values['BYMONTHDAY']['']);
      
  if (is_array($form_values['UNTIL']['datetime'])) {
    $function = $element['#date_repeat_widget'] .'_input_value';
    $until_element = $element;
    $until_element['#value'] = $form_values['UNTIL']['datetime'];
    $form_values['UNTIL']['datetime'] = $function($until_element);
  }
  if (isset($form_values['EXDATE']) && is_array($form_values['EXDATE'])) {
    $function = $element['#date_repeat_widget'] .'_input_value';
    $exdate_element = $element;
    foreach ($form_values['EXDATE'] as $delta => $value) {
      if (is_array($value['datetime'])) {
        $exdate_element['#value'] = $form_values['EXDATE'][$delta]['datetime'];
        $form_values['EXDATE'][$delta]['datetime'] = $function($exdate_element);
      }
    }
  }
  return $form_values;
}

/**
 * Build a RRULE out of the form values.
 */
function date_repeat_rrule_validate($element, &$form_state) {
  require_once('./'. drupal_get_path('module', 'date_api') .'/date_api_ical.inc');
  $form_values = $form_state['values'];
  $field_name = $element['#parents'][0];
  $item = $form_values[$field_name]['rrule'];
  $item = date_repeat_merge($item, $element);
  $rrule = date_api_ical_build_rrule($item);
  form_set_value($element, $rrule, $form_state);
}

/**
 * Theme the exception list as a table so the buttons line up
 */
function theme_date_repeat_current_exceptions($rows = array()) {
  $rows_info = array();
  foreach ($rows as $key => $value) {
    if (substr($key, 0, 1) != '#') {
      $rows_info[] = array(drupal_render($value['action']), drupal_render($value['display']));
    }
  }
  return theme('table', array(t('Delete'), t('Current Exceptions')), $rows_info);
}

/**
 * Themes the date repeat element.
 */
function theme_date_repeat($element) {
  return theme('form_element', $element, '<div class="container-inline">'. drupal_render($element). '</div>');
}