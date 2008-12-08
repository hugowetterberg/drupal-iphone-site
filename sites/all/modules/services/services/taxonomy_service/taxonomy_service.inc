<?php
// $Id: taxonomy_service.inc,v 1.1.2.3 2008/09/06 04:13:08 marcingy Exp $
/**
 * @author Services Dev Team
 * @file
 *  Link general taxonomy functionalities to services module.
 */

/**
 * get terms in vocabulary
 */
function taxonomy_service_get_tree($vid) {
  return taxonomy_get_tree($vid);
}

/**
 * select_nodes
 */
function taxonomy_service_select_nodes($tids = array(), $fields = array(), $operator = 'or', $depth = 0, $pager = TRUE, $order = 'n.sticky DESC, n.created DESC') {
  $result = taxonomy_select_nodes($tids, $operator, $depth, $pager, $order);
  while ($node = db_fetch_object($result)) {
    $nodes[] = services_node_load(node_load($node), $fields);
  }

  return $nodes;
}
