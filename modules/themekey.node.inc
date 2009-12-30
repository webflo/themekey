<?php
// $Id$

function themekey_node_themekey_properties() {
  // Attributes for properties
  $attributes = array();
  $attributes['node:changed'] = array(
    'description' => t('Node: Changed date - The date when the node was last edited/updated, formatted as unix timestamp like "1248873565".'),
    'validator' => 'themekey_validator_ctype_digit',
  );
  $attributes['node:created'] = array(
    'description' => t('Node: Created date - The date when the node was created, formatted as unix timestamp like "1248873565".'),
    'validator' => 'themekey_validator_ctype_digit',
  );
  $attributes['node:changed_date_time'] = array(
    'description' => t('Node: Changed date - The date including the time when the node was last edited/updated, formatted like "2009-12-24 18:13:24"'),
    'validator' => 'themekey_validator_date_time',
  );
  $attributes['node:created_date_time'] = array(
    'description' => t('Node: Created date - The date including the time when the node was created, formatted like "2009-12-24 18:13:24"'),
    'validator' => 'themekey_validator_date_time',
  );
  $attributes['node:changed_date'] = array(
    'description' => t('Node: Changed date - The date without the time when the node was last edited/updated, formatted like "2009-12-24"'),
    'validator' => 'themekey_validator_date',
  );
  $attributes['node:created_date'] = array(
    'description' => t('Node: Created date - The date without the time when the node was created, formatted like "2009-12-24"'),
    'validator' => 'themekey_validator_date',
  );
  $attributes['node:language'] = array(
    'description' => t('Node: Language - The code of the selected language of a node, formatted like "en" or "de". See !link for the codes of your enabled languages', 
      array('!link' => l('admin/settings/language', 'admin/settings/language'))),
    'validator' => 'themekey_validator_language',
  );
  $attributes['node:nid'] = array(
    'description' => t('Node: ID - The id of a node (nid), can be found in the URL of the node, "node/23" or "node/23/edit" (23 = nid)'),
    'validator' => 'themekey_validator_ctype_digit',
  );
  $attributes['node:promote'] = array(
    'description' => t('Node: Promoted - If the node is promoted to the front page. Possible values are "0" for true and "1" for false.'),
    'validator' => 'themekey_validator_nummeric_boolean',
  );
  $attributes['node:sticky'] = array(
    'description' => t('Node: Sticky - If the node is set "Sticky at top of lists". Possible values are "0" for true and "1" for false.'),
    'validator' => 'themekey_validator_nummeric_boolean',
  );
  $attributes['node:type'] = array(
    'description' => t('Node: Type - The machine readable content type of the node. See !link for your content types (use column "Type"). Drupal default types are "page" and "story".', 
      array('!link' => l('admin/content/types', 'admin/content/types'))),
    'validator' => 'themekey_validator_node_type',
  );
  $attributes['node:uid'] = array(
    'description' => t('Node: User ID - The user id (uid) of the author of the node. The user id can be found in the URL of the users profile page, "user/23" or "user/23/edit" (23 = uid). See !link for your users.', 
      array('!link' => l('admin/user/user/list', 'admin/user/user/list'))),
    'validator' => 'themekey_validator_ctype_digit',
  );
  $attributes['node:title'] = array(
    'description' => t('Node: Title - The title of the node.'),
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


