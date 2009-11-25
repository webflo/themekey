<?php
// $Id$

function themekey_node_themekey_properties() {
  // Attributes for properties
  $attributes = array();
  $attributes['node:changed'] = array('description' => t('Node: Changed date'));
  $attributes['node:created'] = array('description' => t('Node: Created date'));
  $attributes['node:language'] = array('description' => t('Node: Language'));
  $attributes['node:nid'] = array('description' => t('Node: ID'));
  $attributes['node:promote'] = array('description' => t('Node: Promoted'));
  $attributes['node:sticky'] = array('description' => t('Node: Sticky'));
  $attributes['node:type'] = array('description' => t('Node: Type'));
  $attributes['node:uid'] = array('description' => t('Node: User ID'));
  $attributes['node:title'] = array('description' => t('Node: Title'));

  // Mapping functions
  $maps = array();
  $maps[] = array('src'       => 'node:nid',
                  'dst'       => 'node:changed',
                  'callback'  => 'themekey_node_nid2changed');
  $maps[] = array('src'       => 'node:nid',
                  'dst'       => 'node:created',
                  'callback'  => 'themekey_node_nid2created');
  $maps[] = array('src'       => 'node:nid',
                  'dst'       => 'node:language',
                  'callback'  => 'themekey_node_nid2language');
  $maps[] = array('src'       => 'node:nid',
                  'dst'       => 'node:promote',
                  'callback'  => 'themekey_node_nid2promote');
  $maps[] = array('src'       => 'node:nid',
                  'dst'       => 'node:sticky',
                  'callback'  => 'themekey_node_nid2sticky');
  $maps[] = array('src'       => 'node:nid',
                  'dst'       => 'node:type',
                  'callback'  => 'themekey_node_nid2type');
  $maps[] = array('src'       => 'node:nid',
                  'dst'       => 'node:uid',
                  'callback'  => 'themekey_node_nid2uid');
  $maps[] = array('src'       => 'node:nid',
                  'dst'       => 'node:title',
                  'callback'  => 'themekey_node_nid2title');

  return array('attributes' => $attributes, 'maps' => $maps);
}

function themekey_node_themekey_paths() {
  $paths = array();
  $paths[] = array('path'       => 'node/#node:nid',
                   'callbacks'  => array('themekey_node_callback'));

  return $paths;
}

function themekey_node_callback(&$item, &$parameters) {
  if ($node = node_load($parameters['node:nid'])) {
    $parameters['theme'] = !empty($node->theme) ? $node->theme : 'default';
  }
}

function themekey_node_get_simple_node_property($nid, $property) {
  // node should already be returned from cache at this point
  if ($node = node_load($nid)) {
    return $node->$property;
  }
  return NULL;
}

function themekey_node_nid2changed($nid) {
  return themekey_node_get_simple_node_property($nid, 'changed');
}

function themekey_node_nid2created($nid) {
  return themekey_node_get_simple_node_property($nid, 'created');
}

function themekey_node_nid2language($nid) {
  return themekey_node_get_simple_node_property($nid, 'language');
}

function themekey_node_nid2promote($nid) {
  return themekey_node_get_simple_node_property($nid, 'promote');
}

function themekey_node_nid2sticky($nid) {
  return themekey_node_get_simple_node_property($nid, 'sticky');
}

function themekey_node_nid2type($nid) {
  return themekey_node_get_simple_node_property($nid, 'type');
}

function themekey_node_nid2uid($nid) {
  return themekey_node_get_simple_node_property($nid, 'uid');
}

function themekey_node_nid2title($nid) {
  return themekey_node_get_simple_node_property($nid, 'title');
}
