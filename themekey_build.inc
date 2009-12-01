<?php
// $Id$

/**
 * Function _themekey_theme_options().
 */
function _themekey_theme_options($default = TRUE) {
  $themes = list_themes();
  ksort($themes);

  $options_themes = array();
  if ($default) {
    $options_themes['default'] = t('System default');
  }
  foreach ($themes as $themex) {
    if ($themex->status || variable_get('themekey_allthemes', 0)) {
      $options_themes[$themex->name] = $themex->info['name'];
    }
  }

  return $options_themes;
}

/**
 * Function _themekey_rebuild().
 */
function _themekey_rebuild() {
  // Create a list of modules in the /modules subfolder (internal modules)
  _themekey_scan_modules();

  // Get property definitions (from internal and other modules)
  $properties = module_invoke_all('themekey_properties');

  // Attributes
  $attributes = isset($properties['attributes']) ? $properties['attributes'] : array();
  variable_set('themekey_attributes', $attributes);

  $property_names = array_keys($attributes);
  sort($property_names);
  $property_names = array_combine($property_names, $property_names);
  variable_set('themekey_properties', $property_names);

  // Property maps
  $maps = isset($properties['maps']) ? $properties['maps'] : array();
  variable_set('themekey_maps', $maps);

  // Find all previous registered paths
  $obsolete_paths = array();
  $result = db_query('SELECT id, path FROM {themekey_paths}');
  while ($item = db_fetch_array($result)) {
    $obsolete_paths[$item['id']] = $item['path'];
  }
  // Get (and register) paths from themekey modules
  $paths = module_invoke_all('themekey_paths');
  foreach ($paths as $item) {
    _themekey_path_set($item);
    if (isset($item['id'])) {
      unset($obsolete_paths[$item['id']]);
    }
  }
  // Unregister obsolete paths
  foreach (array_keys($obsolete_paths) as $id) {
    _themekey_path_del($id);
  }
}

/**
 * Function _themekey_scan_modules().
 */
function _themekey_scan_modules() {
  $modules = array();
  $files = file_scan_directory(drupal_get_path('module', 'themekey') .'/modules', '\themekey.[a-z]+.inc$');
  foreach ($files as $file) {
    list( , $module) = explode('.', $file->name);
    if (module_exists($module)) {
      $modules[] = $file->name;
    }
  }

  variable_set('themekey_modules', $modules);
}

/**
 * Function _themekey_include_modules().
 */
function _themekey_include_modules() {
  foreach (variable_get('themekey_modules', array('themekey.node')) as $module) {
    if (is_readable(drupal_get_path('module', 'themekey') .'/modules/'. $module .'.inc')) {
      require_once(drupal_get_path('module', 'themekey') .'/modules/'. $module .'.inc');
    }
  }
}

/* Path-based */

/**
 * Function themekey_complete_path().
 */
function themekey_complete_path(&$item) {
  $item['wildcards'] = unserialize($item['wildcards']);
  if (count($item['wildcards'])) {
    $parts = explode('/', $item['value'], MENU_MAX_PARTS);
    foreach ($item['wildcards'] as $index => $wildcard) {
      $parts[$index] .= $wildcard;
    }
    $item['value'] = implode('/', $parts);
  }
}

/**
 * Function _themekey_path_set().
 */
function _themekey_path_set(&$item) {
  $item['callbacks'] = (isset($item['callbacks']) && !empty($item['callbacks'])) ? $item['callbacks'] : array();

  list($item['fit'], $item['weight'], $item['wildcards']) = _themekey_prepare_path($item['path']);

  drupal_write_record('themekey_paths', $item, isset($item['id']) ? 'id' : array());
}

/**
 * Function _themekey_path_del().
 */
function _themekey_path_del($id) {
  db_query('DELETE FROM {themekey_paths} WHERE id = %d', $id);
}

/**
 * Function _themekey_prepare_path().
 * (based on _menu_router_build() in includes/menu.inc)
 */
function _themekey_prepare_path(&$path) {
  $fit = 0;
  $weight = 0;
  $wildcards = array();

  $parts = explode('/', $path, MENU_MAX_PARTS);
  $slashes = count($parts) - 1;
  foreach ($parts as $index => $part) {
    if (preg_match('/^(\%|\#)([a-z0-9_:]*)$/', $part, $matches)) {
      $parts[$index] = $matches[1];
      if (!empty($matches[2])) {
        $wildcards[$index] = $matches[2];
      }
      if ($matches[1] == '#') {
        $weight |=  1 << ($slashes - $index);
      }
    }
    else {
      $fit |=  1 << ($slashes - $index);
    }
  }
  $path = implode('/', $parts);

  return array($fit, $weight, $wildcards);
}

/**
 * Function themekey_prepare_custom_path().
 * (based on _menu_router_build() in includes/menu.inc)
 */
function themekey_prepare_custom_path($path) {
  $wildcards = array();

  $parts = explode('/', $path, MENU_MAX_PARTS);
  foreach ($parts as $index => $part) {
    if (preg_match('/^(\%|\#)([a-z0-9_:]*)$/', $part, $matches)) {
      $parts[$index] = $matches[1];
      if (!empty($matches[2])) {
        $wildcards[$index] = $matches[2];
      }
    }
  }
  $path = implode('/', $parts);

  return array($path, $wildcards);
}

/**
 * Function _themekey_properties_explode_conditions().
 */
function _themekey_properties_explode_conditions($conditions) {
  if (!is_array($conditions)) {
    $parts = array_filter(explode(';', $conditions));
    $conditions = array();
    foreach ($parts as $part) {
      $part = trim($part);
      if (preg_match('/(.*?)(<=|<|>=|>|=|!|~)(.*)/', $part, $matches)) {
        $conditions[] = array(
          'property' => trim($matches[1]),
          'operator' => trim($matches[2]),
          'value' => trim($matches[3])
        );
      }
    }
  }

  return $conditions;
}

/**
 * Function _themekey_properties_implode_conditions().
 */
function _themekey_properties_implode_conditions($conditions) {
  $string = array();
  foreach ($conditions as $condition) {
    $string[] = $condition['property'] . $condition['operator'] . $condition['value'];
  }
  $string = implode('; ', $string);

  return $string;
}

/**
 * Function _themekey_load_properties().
 */
function _themekey_load_properties($limit = NULL) {
  $properties = array();
  $query = 'SELECT * FROM {themekey_properties} ORDER BY weight';
  $result = isset($limit) ? pager_query($query, $limit) : db_query($query);
  while ($item = db_fetch_array($result)) {
    $item['conditions'] = _themekey_properties_implode_conditions(unserialize($item['conditions']));
    if ('drupal:path' == $item['property']) {
      themekey_complete_path($item);
    }
    $properties[] = $item;
  }

  return $properties;
}

/**
 * Function _themekey_properties_set().
 */
function _themekey_properties_set(&$item) {
  $item['conditions'] = (isset($item['conditions']) && !empty($item['conditions'])) ? $item['conditions'] : array();
  $item['conditions'] = _themekey_properties_explode_conditions($item['conditions']);

  if ('drupal:path' == $item['property']) {
    list($item['value'], $item['wildcards']) = themekey_prepare_custom_path($item['value']);
  }
  else {
    $item['wildcards'] = array();
  }

  drupal_write_record('themekey_properties', $item, isset($item['id']) ? 'id' : array());
}

/**
 * Function _themekey_properties_del().
 */
function _themekey_properties_del($id) {
  db_query('DELETE FROM {themekey_properties} WHERE id = %d', $id);
}

/**
 * Function _themekey_properties_resort().
 */
function _themekey_properties_resort($key) {
  $attributes = variable_get('themekey_attributes', array());
  if (isset($attributes[$key]['multiple']) && $attributes[$key]['multiple']) {
    _themekey_include_modules();
    $weightfn = $attributes[$key]['weight'];
    $result = db_query('SELECT * FROM {themekey_properties} WHERE property = \'%s\'', $key);
    while ($item = db_fetch_array($result)) {
      $item['weight'] = function_exists($weightfn) ? $weightfn($item) : 0;
      drupal_write_record('themekey_properties', $item, 'id');
    }
  }
}
