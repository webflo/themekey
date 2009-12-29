<?php
// $Id$

/**
 * @file
 * helper functions for ThemeKey UI feature to add a theme option to the
 * 'URL aliases' administration if module "Path" is enabled.
 *
 * @see themekey_ui.module
 *
 * @author Markus Kalkbrenner | Cocomore AG
 *   @see http://drupal.org/user/124705
 *
 * @author profix898
 *   @see http://drupal.org/user/35192
 */

require_once(drupal_get_path('module', 'themekey') .'/themekey_build.inc');

/**
 * Returns a theme assigned to a drupal path using
 * a ThemeKey Theme Switching Rule which is not part of a chain
 *
 * @param $path
 *   drupal path as string
 *
 * @return
 *   array with to elements:
 *   - id of the database entry in table themekey_properties
 *     or '0' if no entry exists
 *   - the theme
 */
function themekey_ui_get_path_theme($path) {
  $serialized_empty_array = serialize(array());
  if ($result = db_query("SELECT id, theme FROM {themekey_properties} WHERE property = 'drupal:path' AND operator = '=' AND value = '%s' AND wildcards = '%s' AND parent = 0", $path, $serialized_empty_array)) {
    while ($row = db_fetch_array($result)) {
      if (!db_result(db_query("SELECT COUNT(*) FROM {themekey_properties} WHERE parent = %d", $row['id']))) {
        return array($row['id'], $row['theme']);
      }
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
