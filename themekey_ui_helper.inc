<?php
// $Id$

require_once(drupal_get_path('module', 'themekey') .'/themekey_build.inc');

/**
 * Function themekey_ui_get_path_theme().
 */
function themekey_ui_get_path_theme($path) {
  $serialized_empty_array = serialize(array());
  if ($result = db_query("SELECT id, theme FROM {themekey_properties} WHERE property = 'drupal:path' AND operator = '=' AND value = '%s' AND wildcards = '%s'", $path, $serialized_empty_array)) {
    if ($row = db_fetch_array($result)) {
      return array($row['id'], $row['theme']);
    }
  }

  return array(0, 'default');
}

/**
 * Function themekey_ui_set_path_theme().
 */
function themekey_ui_set_path_theme($path, $theme = 'default', $id = 0) {
  $item = array(
    'property' => 'drupal:path',
    'value' => $path,
    'theme' => $theme,
    'enabled' => 1,
  );

  if ($id > 0) {
    $item['id'] = $id;
  }

  _themekey_properties_set($item);
}

/**
 * Function themekey_ui_del_path_theme().
 */
function themekey_ui_del_path_theme($id) {
  return _themekey_properties_del($id);
}
