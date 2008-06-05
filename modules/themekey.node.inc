<?php
// $Id$

function themekey_node_themekey_properties() {
  // Attributes for properties
  $attributes = array();
  $attributes['nid'] = array('description' => t('Node: ID'));

  return array('attributes' => $attributes);
}

function themekey_node_themekey_paths() {
  $paths = array();
  $paths[] = array('path'       => 'node/#nid',
                   'callbacks'  => array('_themekey_node_callback'));
  
  return $paths;
}

function _themekey_node_callback(&$item, $params) {
  $node = node_load($params['nid']);
  if ($theme = _themekey_match_properties($node)) {
    $item['theme'] = $theme;
  }
}
