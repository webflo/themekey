<?php
// $Id$

function themekey_taxonomy_themekey_properties() {
  // Attributes for properties ;)
  $attributes = array();
  $attributes['taxonomy:vid'] = array('multiple' => TRUE,
                                      'path' => FALSE,
                                      'weight' => 'themekey_taxonomy_vid_weight',
                                      'description' => t('Taxonomy: Vocabulary'));
  $attributes['taxonomy:tid'] = array('multiple' => TRUE,
                                      'path' => 'taxonomy',
                                      'weight' => 'themekey_taxonomy_tid_weight',
                                      'description' => t('Taxonomy: Term'));
  $attributes['taxonomy:tid_and_childs'] = array('multiple' => TRUE,
                                      'path' => 'taxonomy',
                                      'weight' => 'themekey_taxonomy_tid_weight',
                                      'description' => t('Taxonomy: Term and its childs'));

  // Mapping functions
  $maps = array();
  $maps[] = array('src'       => 'taxonomy:tid',
                  'dst'       => 'taxonomy:vid',
                  'callback'  => 'themekey_taxonomy_tid2vid');
  $maps[] = array('src'       => 'node:nid',
                  'dst'       => 'taxonomy:vid',
                  'callback'  => 'themekey_taxonomy_nid2vid');
  $maps[] = array('src'       => 'node:nid',
                  'dst'       => 'taxonomy:tid',
                  'callback'  => 'themekey_taxonomy_nid2tid');
  $maps[] = array('src'       => 'node:nid',
                  'dst'       => 'taxonomy:tid_and_childs',
                  'callback'  => 'themekey_taxonomy_nid2tid_and_parents');

  return array('attributes' => $attributes, 'maps' => $maps);
}

function themekey_taxonomy_themekey_paths() {
  $paths = array();
  $paths[] = array('path' => 'taxonomy/term/#taxonomy:tid');

  // Add support for 'forum' paths
  if (module_exists('forum')) {
    $paths[] = array('path' => 'forum/#taxonomy:vid');
  }
  // Add support for 'taxonomy_menu' paths
  if (module_exists('taxonomy_menu')) {
    $prefix = variable_get('taxonomy_menu_display_page', 'category');
    $paths[] = array('path' => $prefix .'/#taxonomy:vid/#taxonomy:tid');
    for ($i=1; $i<=MENU_MAX_PARTS-3; $i++) {
      $paths[] = array('path' => $prefix .'/#taxonomy:vid/'. implode('/', array_fill(0, $i, '#')) .'/#taxonomy:tid');
    }
  }

  return $paths;
}

function themekey_taxonomy_vid_weight($item) {
  return db_result(db_query('SELECT weight FROM {vocabulary} WHERE vid = %d', $item['value']));
}

function themekey_taxonomy_tid_weight($item) {
  return db_result(db_query('SELECT weight FROM {term_data} WHERE tid = %d', $item['value']));
}

function themekey_taxonomy_tid2vid($tids) {
  $vid = array();
  // TODO use taxonomy API instead SQL
  $tids = is_array($tids) ? $tids : array($tids);
  foreach ($tids as $tid) {
    $vid[] = db_result(db_query('SELECT vid FROM {term_data} WHERE tid = %d', $tid));
  }

  return count($vid) ? $vid : FALSE;
}

function themekey_taxonomy_nid2vid($nid, $object = NULL) {
  $vid = array();
  // TODO use taxonomy API instead SQL
  $result = db_query('SELECT td.vid FROM {term_data} td INNER JOIN {term_node} tn ON td.tid = tn.tid WHERE tn.nid = %d', $nid);
  while ($term = db_fetch_object($result)) {
    $vid[] = $term->vid;
  }

  return count($vid) ? $vid : FALSE;
}

function themekey_taxonomy_nid2tid($nid, $object = NULL) {
  $tid = array();
  // TODO use taxonomy API instead SQL
  $result = db_query('SELECT tid FROM {term_node} WHERE nid = %d', $nid);
  while ($term = db_fetch_object($result)) {
    $tid[] = $term->tid;
  }

  return count($tid) ? $tid : FALSE;
}

function themekey_taxonomy_nid2tid_and_parents($nid, $object = NULL) {
  $node_tids = themekey_taxonomy_nid2tid($nid, $object);
  $tids = $node_tids;
  foreach ($node_tids as $tid) {
    $parent_terms = taxonomy_get_parents_all($tid);
    foreach ($parent_terms as $parent_term) {
      $tids[] = $parent_term->tid;
    }
  }

  return count($tids) ? array_unique($tids) : FALSE;
}
