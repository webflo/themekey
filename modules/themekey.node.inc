<?php
// $Id$

function themekey_node_themekey_properties() {
  // Attributes for properties
  $attributes = array();
  $attributes['node:changed'] = array(
    'description' => t('Node: Changed date'),
    'validator' => 'themekey_validator_ctype_digit',
  );
  $attributes['node:created'] = array(
    'description' => t('Node: Created date'),
    'validator' => 'themekey_validator_ctype_digit',
  );
  $attributes['node:changed_date_time'] = array(
    'description' => t('Node: Changed date formatted like "2009-12-24 18:13:24"'),
    'validator' => 'themekey_validator_date_time',
  );
  $attributes['node:created_date_time'] = array(
    'description' => t('Node: Created date formatted like "2009-12-24 18:13:24"'),
    'validator' => 'themekey_validator_date_time',
  );
  $attributes['node:changed_date'] = array(
    'description' => t('Node: Changed date formatted like "2009-12-24"'),
    'validator' => 'themekey_validator_date',
  );
  $attributes['node:created_date'] = array(
    'description' => t('Node: Created date formatted like "2009-12-24"'),
    'validator' => 'themekey_validator_date',
  );
  $attributes['node:language'] = array(
    'description' => t('Node: Language'),
    'validator' => 'themekey_validator_language',
  );
  $attributes['node:nid'] = array(
    'description' => t('Node: ID'),
    'validator' => 'themekey_validator_ctype_digit',
  );
  $attributes['node:promote'] = array(
    'description' => t('Node: Promoted'),
    'validator' => 'themekey_validator_nummeric_boolean',
  );
  $attributes['node:sticky'] = array(
    'description' => t('Node: Sticky'),
    'validator' => 'themekey_validator_nummeric_boolean',
  );
  $attributes['node:type'] = array(
    'description' => t('Node: Type'),
    'validator' => 'themekey_validator_node_type',
  );
  $attributes['node:uid'] = array(
    'description' => t('Node: User ID'),
    'validator' => 'themekey_validator_ctype_digit',
  );
  $attributes['node:title'] = array(
    'description' => t('Node: Title'),
    'validator' => '',
  );

  // Mapping functions
  $maps = array();
  $maps[] = array('src'       => 'node:nid',
                  'dst'       => 'node:changed',
                  'callback'  => 'themekey_node_nid2changed');
  $maps[] = array('src'       => 'node:nid',
                  'dst'       => 'node:created',
                  'callback'  => 'themekey_node_nid2created');
  $maps[] = array('src'       => 'node:changed',
                  'dst'       => 'node:changed_date_time',
                  'callback'  => 'themekey_node_timestamp2datetime');
  $maps[] = array('src'       => 'node:created',
                  'dst'       => 'node:created_date_time',
                  'callback'  => 'themekey_node_timestamp2datetime');
  $maps[] = array('src'       => 'node:changed',
                  'dst'       => 'node:changed_date',
                  'callback'  => 'themekey_node_timestamp2date');
  $maps[] = array('src'       => 'node:created',
                  'dst'       => 'node:created_date',
                  'callback'  => 'themekey_node_timestamp2date');
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
  $paths[] = array('path' => 'node/#node:nid');

  return $paths;
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

function themekey_node_timestamp2datetime($timestamp) {
  return date('Y-m-d H:i:s', $timestamp);
}

function themekey_node_timestamp2date($timestamp) {
  return date('Y-m-d', $timestamp);
}

/**
 * This function isn't needed anymore and has been removed with ThemeKey 6.x-2.0-beta2.
 * But if it's missing a fatal error occurs if you upgrade from any version before
 * 6.x-2.0-beta2 until you called update.php. That should be avoided.
 * So this function is available again and does nothing else than reminding the
 * administrator to call update.php
 *
 * see http://drupal.org/node/662786
 *
 * @param unknown_type $item
 * @param unknown_type $parameters
 * @return unknown_type
 */
function _themekey_node_callback(&$item, &$parameters) {
  global $user;

  if (1 == $user->uid) {
    drupal_set_message(t('You uploaded a new version of ThemeKey. Please !link your site!', l(t('update'), '/update.php')));
  }
}


