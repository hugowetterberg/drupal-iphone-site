<?php
// $Id: search_service.inc,v 1.1.2.2 2008/09/01 14:57:48 brmassa Exp $
/**
 * @author Services Dev Team
 * @file
 *  Link general search functionalities to services module.
 */

/**
 * Callback for search.nodes service.
 */
function search_service_nodes($keys, $simple = FALSE) {
  // define standard keys for simple set
  $stdkeys = array('link', 'type', 'title', 'user', 'date', 'snippet');

  // invoke the search hook to generate results
  $results = module_invoke('node', 'search', 'search', $keys);
  if ($results and is_array($results) and count($results)) {
    // if simple results requested, remove extra data
    if ($simple) {
      $num = count($results);
      for ($i = 0; $i<$num; $i++) {
        $keys = array_keys($results[$i]);
        foreach ($keys as $key) {
          if (!in_array($key, $stdkeys)) {
            unset($results[$i][$key]);
          }
        }
      }
    }
    return $results;
  }
  return services_error(t('Search returned no results.'));
}

/**
 * Callback for search.users service.
 */
function search_service_users($keys) {
  // invoke the search hook to generate results
  $results = module_invoke('user', 'search', 'search', $keys);
  if ($results and is_array($results) and count($results)) {
    return $results;
  }
  return services_error(t('Search returned no results.'));
}
