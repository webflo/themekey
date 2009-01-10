<?php
// $Id$

/**
 * Function _themekey_theme_options().
 */
function _themekey_theme_options($default = TRUE) {
  $themes = system_theme_data();
  ksort($themes);
  $options_themes = array();
  if ($default) {
    $options_themes['default'] = t('System default');
  }
  foreach ($themes as $themex) {
    if ($themex->status || variable_get('themekey_allthemes', 1)) {
      $options_themes[$themex->name] = $themex->info['name'];
    }
  }

  return $options_themes;
}

/**
 * Function _themekey_rebuild().
 */
function _themekey_rebuild() {
  _themekey_scan_modules();
  
  //
  $properties = module_invoke_all('themekey_properties');
  //
  $attributes = isset($properties['attributes']) ? $properties['attributes'] : array();
  variable_set('themekey_attributes', $attributes);
  //
  $maps = isset($properties['maps']) ? $properties['maps'] : array();
  drupal_alter('themekey_maps', $maps);
  variable_set('themekey_maps', $maps);
  
  //
  $oldpaths = array();
  $result = db_query('SELECT id, path FROM {themekey_paths} WHERE custom = 0');
  while ($item = db_fetch_array($result)) {
    $oldpaths[$item['id']] = $item['path'];
  }
  //
  $paths = module_invoke_all('themekey_paths');
  drupal_alter('themekey_paths', $paths);
  foreach ($paths as $item) {
    _themekey_path_set($item, FALSE);
    if (isset($item['id'])) {
      unset($oldpaths[$item['id']]);
    }
  }
  //
  foreach (array_keys($oldpaths) as $id) {
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
 * Function _themekey_invoke_modules().
 */
function _themekey_invoke_modules($hook) {
  $return = array();
  foreach (variable_get('themekey_modules', array('themekey.node')) as $module) {
    $function = str_replace('.', '_', $module) .'_'. $hook;
    if (function_exists($function)) {
      $return = array_merge_recursive($return, $function());
    }
  }

  return $return;
}

/* Path-based */

/**
 * Function _themekey_load_paths().
 */
function _themekey_load_paths($limit = NULL) {
  $paths = array();
  $query = 'SELECT * FROM {themekey_paths} WHERE custom = 1';
  $result = isset($limit) ? pager_query($query, $limit) : db_query($query);
  while ($item = db_fetch_array($result)) {
    $item['wildcards'] = unserialize($item['wildcards']);
    if (count($item['wildcards'])) {
      $parts = explode('/', $item['path'], MENU_MAX_PARTS);
      foreach ($item['wildcards'] as $index => $wildcard) {
        $parts[$index] .= $wildcard;
      }
      $item['path'] = implode('/', $parts);
    }
    $item['conditions'] = unserialize($item['conditions']);
    $item['conditions'] = count($item['conditions']) ? implode('; ', $item['conditions']) : '';
    $paths[] = $item;
  }

  return $paths;
}

/**
 * Function _themekey_path_set().
 */
function _themekey_path_set(&$item, $custom = TRUE) {
  $item['callbacks'] = (isset($item['callbacks']) && !empty($item['callbacks'])) ? $item['callbacks'] : array();
  $item['conditions'] = (isset($item['conditions']) && !empty($item['conditions'])) ? $item['conditions'] : array();
  if (!is_array($item['conditions'])) {
    $item['conditions'] = preg_split('/;[\s?]/', $item['conditions']);
  }
  list($item['fit'], $item['weight'], $item['wildcards']) = _themekey_prepare_path($item['path']);
  $item['custom'] = $custom;
  if (!isset($item['id'])) {
    if ($id = db_result(db_query('SELECT id FROM {themekey_paths} WHERE path = \'%s\' AND custom = %d', $item['path'], $item['custom']))) {
      $item['id'] = $id;
    }
  }

  drupal_write_record('themekey_paths', $item, isset($item['id']) ? 'id' : array());
}

/**
 * Function _themekey_path_del().
 */
function _themekey_path_del($id) {
  db_query('DELETE FROM {themekey_paths} WHERE id = %d', $id);
}

/**
 * Function _themekey_path_clear().
 */
function _themekey_path_clear() {
  db_query('DELETE FROM {themekey_paths} WHERE custom = 1');
}

/**
 * Function themekey_num_paths().
 */
function themekey_num_paths() {
  return db_result(db_query('SELECT COUNT(id) FROM {themekey_paths} WHERE custom = 1'));
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
    if (preg_match('/^(\%|\#)([a-z0-9_]*)$/', $part, $matches)) {
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

/* Property-based */

/**
 * Function _themekey_node_discover().
 */
function _themekey_node_discover() {
  require_once(drupal_get_path('module', 'node') .'/node.pages.inc');

  $properties = array();
  $ignore = array(// Generic node properties
                  'teaser', 'teaser_js', 'teaser_include', 'body', 'format', 'log', 'submit', 'preview',
                  'form_id', 'path', 'vid', 'validated',
                  // Book module properties
                  'book/router_path', 'book/module', 'book/parent_depth_limit', 'book/weight', 'book/pick-book',
                  'book/menu_name', 'book/has_children', 'book/original_bid',
                  // Menu properties
                  'menu/module', 'menu/parent_depth_limit', 'menu/weight', 'menu/customized', 'menu/expanded',
                  'menu/has_children', 'menu/hidden'
                 );
  //
  $nodes = array();
  $node_types = node_get_types();
  //
  foreach ($node_types as $node_type) {
    $form_state = array();
    $form_id = $node_type->type .'_node_form';
    $node = array('uid' => 0, 'name' => '-', 'type' => $node_type->type, 'language' => 'en');
    //
    $form = drupal_retrieve_form($form_id, $form_state, $node);
    $form['#post'] = array();
    drupal_prepare_form($form_id, $form, $form_state);
    $form = form_builder($form_id, $form, $form_state);
    $node = node_form_submit_build_node($form, $form_state);
    //
    _themekey_node_discover_properties($node, $properties);
    $properties = array_diff($properties, $ignore);
  }

  return $properties;
}

/**
 * Function _themekey_node_discover_properties().
 */
function _themekey_node_discover_properties($node, &$properties, $parents = array()) {
  foreach ($node as $property => $value) {
    if (!is_numeric($property) && !array_key_exists($property, $properties)) {
      if (is_array($value)) {
        array_push($parents, $property);
        _themekey_node_discover_properties($value, $properties, $parents);
        array_pop($parents);
      }
      else {
        $element = (count($parents) ? (implode('/', $parents) .'/') : '') . $property;
        $properties[$property] = $element;
      }
    }
  }
}

/**
 * Function _themekey_properties_discover().
 */
function _themekey_properties_discover($nodediscover = FALSE) {
  $properties = array();
  // Module-exposed properties
  $attributes = variable_get('themekey_attributes', array());
  foreach (array_keys($attributes) as $property) {
    $properties[$property] = $property;
  }
  // URL argument properties
  $result = db_query('SELECT wildcards FROM {themekey_paths} WHERE custom = 0');
  while ($item = db_fetch_array($result)) {
    $item['wildcards'] = unserialize($item['wildcards']);
    foreach ($item['wildcards'] as $wildcard) {
      if (!array_key_exists($wildcard, $properties)) {
        $properties[$wildcard] = $wildcard;
      }
    }
  }
  // Node properties
  if ($nodediscover) {
    $properties = array_merge($properties, _themekey_node_discover());
  }
  
  return $properties;
}

/**
 * Function _themekey_properties_explode_conditions().
 */
function _themekey_properties_explode_conditions($conditions) {
  if (!is_array($conditions)) {
    $parts = array_filter(explode(';', $conditions));
    $conditions = array();
    foreach ($parts as $part) {
      list($key, $value) = explode('=', $part);
      $conditions[] = array('property' => trim($key), 'value' => trim($value));
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
    $string[] = $condition['property'] .'='. $condition['value'];
  }
  $string = implode('; ', $string);

  return $string;
}

/**
 * Function _themekey_load_properties().
 */
function _themekey_load_properties($limit = NULL, $order = TRUE) {
  $properties = array();
  $query = 'SELECT * FROM {themekey_properties}';
  $query .= $order ? ' ORDER BY weight' : '';
  $result = isset($limit) ? pager_query($query, $limit) : db_query($query);
  while ($item = db_fetch_array($result)) {
    $item['conditions'] = _themekey_properties_implode_conditions(unserialize($item['conditions']));
    $properties[] = $item;
  }

  return $properties;
}

/**
 * Function _themekey_properties_set().
 */
function _themekey_properties_set(&$item) {
  if (preg_match('/'. $item['property'] .'=(.*)/', $item['value'], $matches)) {
    $item['value'] = $matches[1];
  }
  $item['conditions'] = (isset($item['conditions']) && !empty($item['conditions'])) ? $item['conditions'] : array();
  $item['conditions'] = _themekey_properties_explode_conditions($item['conditions']);
  $item['callbacks'] = (isset($item['callbacks']) && !empty($item['callbacks'])) ? $item['callbacks'] : array();
  //
  $attributes = variable_get('themekey_attributes', array());
  if (isset($attributes[$item['property']]['multiple']) && $attributes[$item['property']]['multiple']) {
    $weightfn = $attributes[$item['property']]['weight'];
    $item['weight'] = function_exists($weightfn) ? $weightfn($item) : 0;
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
 * Function _themekey_properties_delall().
 */
function _themekey_properties_delall($properties) {
  $properties = is_array($properties) ? $properties : array($properties);
  foreach($properties as $property) {
    db_query('DELETE FROM {themekey_properties} WHERE property = \'%s\'', $property);
  }
}

/**
 * Function _themekey_properties_clear().
 */
function _themekey_properties_clear() {
  db_query('DELETE FROM {themekey_properties}');
}

/**
 * Function _themekey_properties_cmp().
 */
function _themekey_properties_cmp($a, $b) {
  if ($a['weight'] == $b['weight']) {
    return ($a['path'] < $b['path']) ? -1 : 1;
  }

  return ($a['weight'] < $b['weight']) ? -1 : 1;
}

/**
 * Function _themekey_properties_resort().
 */
function _themekey_properties_resort($key) {
  $attributes = variable_get('themekey_attributes', array());
  if (isset($attributes[$key]['multiple']) && $attributes[$key]['multiple']) {
    $weightfn = $attributes[$key]['weight'];
    $result = db_query('SELECT * FROM {themekey_properties} WHERE property = \'%s\'', $key);
    while ($item = db_fetch_array($result)) {
      $item['weight'] = function_exists($weightfn) ? $weightfn($item) : 0;
      drupal_write_record('themekey_properties', $item, 'id');
    }
  }
}
