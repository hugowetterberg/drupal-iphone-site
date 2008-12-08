<?php
// $Id: date_handler_field_multiple.inc,v 1.1.2.5 2008/12/02 17:44:16 karens Exp $

/**
 * @file
 * An extended subclass for field handling that adds multiple field grouping.
 *
 * Fields that want multiple value grouping options in addition to basic
 * field and formatter handling can extend this class.
 */
class date_handler_field_multiple extends content_handler_field_multiple {
  
  function pre_render($values) {
    
    // If there are no values to render (displaying a summary, or query returned no results),
    // or if this is not a grouped field, do nothing specific.
    if (isset($this->view->build_info['summary']) || empty($values) || !$this->defer_query) {
      return parent::pre_render($values);
    }
    
    $field = $this->content_field;
    $db_info = content_database_info($field);
    $options = $this->options;
    $this->view->date_info->date_handler_fields = date_handler_fields($this->view);

    // Build the list of vids to retrieve.
    // TODO: try fetching from cache_content first ??
    $vids = array();
    $this->field_values = array();
    foreach ($values as $result) {
      if (isset($result->{$this->field_alias})) {
        $vids[] = $result->{$this->field_alias};
      }
    }

    // List columns to retrieve.
    $alias = content_views_tablename($field);
    // Prefix aliases with '_' to avoid clashing with field columns names.
    $query_columns = array(
      'node.vid AS _vid',
      "$alias.delta as _delta",
      // nid is needed to generate the links for 'link to node' option.
      'node.nid AS _nid',
    );
    // The actual field columns.
    foreach ($db_info['columns'] as $column => $attributes) {
      $query_columns[] = "$alias.$attributes[column] AS $column";
      $query_fields[] = "$alias.$attributes[column]";
    }
    // If the view has a minimum/maximum date, limited the grouped
    // values to the date range.
    $where = '';
    if (!empty($this->view->date_info->min_date)) {
      $date_handler = date_field_get_sql_handler($field);
      foreach ($this->view->date_info->date_handler_fields as $key => $value) {
        $sql_field = $query_fields[0];
        $sql1 = $date_handler->sql_where_date('DATE', $sql_field, ">=", date_format($this->view->date_info->min_date, DATE_FORMAT_DATETIME));
        $sql2 = $date_handler->sql_where_date('DATE', $sql_field, "<=", date_format($this->view->date_info->max_date, DATE_FORMAT_DATETIME));
        $where[] = $sql1 .' AND '. $sql2;
      }
    }
    $query = 'SELECT '. implode(', ', $query_columns) .
             ' FROM {'. $db_info['table'] ."} $alias".
             " LEFT JOIN {node} node ON node.vid = $alias.vid".
             " WHERE node.vid IN (". implode(',', $vids) .') AND '. implode(' OR ', $where) .
             " ORDER BY node.nid ASC, $alias.delta ". ($options['multiple']['multiple_reversed'] ? 'DESC' : 'ASC');
    $result = db_query($query);

    while ($item = db_fetch_array($result)) {
      // Clean up the $item from vid and delta. We keep nid for now.
      $vid = $item['_vid'];
      unset($item['_vid']);
      $delta = !empty($item['_delta']) ? $item['_delta'] : 0;
      $item['#delta'] = $item['_delta'];
      unset($item['_delta']);
      $this->field_values[$vid][$delta] = $item;
    }
  }

  function render($values) {
    
    // By this time $values is a pseudo node that will be passed
    // to the theme. Add view information to it.
    $values->date_info = $this->view->date_info;
    $values->date_info->date_handler_fields = date_handler_fields($this->view);
    
    return parent::render($values);
  }

}