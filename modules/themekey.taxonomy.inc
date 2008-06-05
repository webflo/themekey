<?php
// $Id$

function themekey_taxonomy_themekey_properties() {
  // Mapping functions
  $maps = array();
  $maps[] = array('src'       => 'tid',
                  'dst'       => 'vid',
                  'callback'  => 'themekey_taxonomy_tid2vid');
  $maps[] = array('src'       => 'nid',
                  'dst'       => 'vid',
                  'callback'  => 'themekey_taxonomy_nid2vid');
  $maps[] = array('src'       => 'nid',
                  'dst'       => 'tid',
                  'callback'  => 'themekey_taxonomy_nid2tid');

  // Attributes for properties ;)
  $attributes = array();
  $attributes['vid'] = array('multiple' => TRUE,
                             'weight' => 'themekey_taxonomy_vid_weight',
                             'description' => t('Taxonomy: Vocabulary'));
  $attributes['tid'] = array('multiple' => TRUE,
                             'weight' => 'themekey_taxonomy_tid_weight',
                             'description' => t('Taxonomy: Term'));

  return array('maps' => $maps, 'attributes' => $attributes);
}

function themekey_taxonomy_themekey_paths() {
  $paths = array();
  $paths[] = array('path' => 'taxonomy/term/#tid');
  $paths[] = array('path' => 'forum/#tid');
  
  // Add support for 'forum' paths
  if (module_exists('forum')) {
    $paths[] = array('path' => 'forum/#vid');
  }
  
  // Add support for 'Taxonomy Menu' paths
  if (module_exists('taxonomy_menu')) {
    $path_prefix = variable_get('taxonomy_menu_display_page', 'category');
    $paths[] = array('path' => $path_prefix .'/#vid');
    $paths[] = array('path' => $path_prefix .'/#vid/#tid');
  }
  
  return $paths;
}

function themekey_taxonomy_vid_weight($item) {
  return db_result(db_query('SELECT weight FROM {vocabulary} WHERE vid = %d', $item['value']));
}

function themekey_taxonomy_tid_weight($item) {
  return db_result(db_query('SELECT weight FROM {term_data} WHERE tid = %d', $item['value']));
}

function themekey_taxonomy_tid2vid($tid) {
  $vid = db_result(db_query('SELECT vid FROM {term_node} WHERE tid = %d', $tid));
  return $vid ? $vid : FALSE;
}

function themekey_taxonomy_nid2vid($nid, $object = NULL) {
  $vid = FALSE;
  if (is_object($object) && isset($object->taxonomy)) {
    foreach ($object->taxonomy as $term) {
      $vid[] = $term->vid;
    }
  }
  else {
    $result = db_query('SELECT vid FROM {term_node} WHERE nid = %d', $nid);
    while ($term = db_fetch_object($result)) {
      $vid[] = $term->vid;
    }
  }
  
  return $vid;
}

function themekey_taxonomy_nid2tid($nid, $object = NULL) {
  if (is_object($object) && isset($object->taxonomy)) {
    return array_keys((array)$object->taxonomy);
  }
  
  $tid = FALSE;
  $result = db_query('SELECT tid FROM {term_node} WHERE nid = %d', $nid);
  while ($term = db_fetch_object($result)) {
    $tid[] = $term->tid;
  }
  
  return $tid;
}
