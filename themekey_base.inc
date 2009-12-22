<?php
// $Id$

define('PATH_MAX_PARTS', 10);

/**
 * Function _themekey_invoke_modules().
 */
function _themekey_invoke_modules($hook) {
  $return = array();
  foreach (variable_get('themekey_modules', array('themekey.node')) as $module) {
    if (is_readable(drupal_get_path('module', 'themekey') .'/modules/'. $module .'.inc')) {
      require_once(drupal_get_path('module', 'themekey') .'/modules/'. $module .'.inc');
    }
    $function = str_replace('.', '_', $module) .'_'. $hook;
    if (function_exists($function)) {
      $return = array_merge_recursive($return, $function());
    }
  }

  return $return;
}

/**
 * Function _themekey_get_path_ancestors().
 */
function _themekey_get_path_ancestors($path) {
  static $path_ancestors = array();

  if (empty($path_ancestors[$path])) {
    $ancestors = array($path);

    // REVIEW Is there a more efficient algorithm?
    for ($i = 0; $i < count($ancestors); $i++) {
       $ancestors = array_unique(array_merge($ancestors, themekey_add_path_ancestors($ancestors[$i])));
    }

    $path_ancestors[$path] = array($ancestors, array_fill(0, count($ancestors), "'%s'"));
  }

  return $path_ancestors[$path];
}

function themekey_add_path_ancestors($path) {
  $ancestors = array();
  $parts = explode('/', $path, PATH_MAX_PARTS);
  foreach ($parts as $key => $part) {
    if ('#' == $part || '%' == $part) {
      continue;
    }

    $slices = array_slice($parts, 0, $key);
    if (!empty($slices)) {
      $ancestors[] = implode('/', $slices);
    }

    if (is_numeric($part) && $part == (int) $part) {
      $ancestors[] = implode('/', array_merge($slices, array('#'), array_slice($parts, $key + 1)));
    }

    $slices[] = '%';
    $ancestors[] = implode('/', $slices);
    $ancestors[] = implode('/', array_merge($slices, array_slice($parts, $key + 1)));
  }
  return array_unique($ancestors);
}

/**
 * Function _themekey_match_paths().
 */
function _themekey_get_global_parameters() {
  static $global_parameters = NULL;

  if (is_null($global_parameters)) {
    $global_parameters = module_invoke_all('themekey_global');

    list($ancestors, $placeholders) = _themekey_get_path_ancestors($_GET['q']);

    // TODO don't store paths in database. mayby use a variable.
    $result = db_query('SELECT * FROM {themekey_paths} WHERE path IN ('. implode(',', $placeholders) .') ORDER BY fit DESC, weight DESC', $ancestors);
    while ($item = db_fetch_array($result)) {
      $wildcards = unserialize($item['wildcards']);
      foreach ($wildcards as $index => $wildcard) {
        $global_parameters[$wildcard] = arg($index, $path);
      }

      $callbacks = unserialize($item['callbacks']);
      if (count($callbacks)) {
        foreach ($callbacks as $callback) {
          $callback($item, $global_parameters);
        }
      }
    }
  }

  return $global_parameters;
}

/**
 * Function _themekey_match_properties().
 */
function _themekey_match_properties() {
  $parameters = _themekey_get_global_parameters();

  return themekey_match_property_childs($parameters);
}

/**
 *
 * @param mixed $parameters
 * @param int $parent
 * @return NULL in case of an error
 *         string name of the theme
 *         FALSE if no more child prperties in the chain
 */
function themekey_match_property_childs(&$parameters, $parent = 0) {
  static $child_lookups = array();

  if (isset($child_lookups[$parent])) {
    // prevent endless recursion in case of malformated data in database
    return $child_lookups[$parent];
  }

  if ($result = db_query("SELECT * FROM {themekey_properties} WHERE enabled = 1 AND parent = %d AND value != '' ORDER BY weight", $parent)) {
    $num_childs = 0;

    while ($item = db_fetch_array($result)) {
      $num_childs++;
      if (_themekey_match_condition($item, $parameters)) {
        if ('drupal:path' == $item['property']) {
          $wildcards = unserialize($item['wildcards']);
          if (!empty($wildcards)) {
            $path = $_GET['q'];
            if (!empty($parameters['internal:path_alias']) && !empty($parameters['internal:alias_uri'])) {
              if (in_array($value, $parameters['internal:path_alias'])) {
                $path = $parameters['internal:alias_uri'];
              }
            }
            foreach ($wildcards as $index => $wildcard) {
              $parameters[$wildcard] = arg($index, $path);
            }
          }
        }

        $child_lookups[$parent] = themekey_match_property_childs($parameters, $item['id']);

        if (FALSE === $child_lookups[$parent]) {
          if (!_themekey_check_theme_enabled($item['theme'])) {
            continue;
          }

          $child_lookups[$parent] = $item['theme'];
        }

        // return own theme or theme from child property or NULL in case of a database error
        return $child_lookups[$parent];
      }
    }

    // No more child properties in the chain
    if (0 == $num_childs) {
      $child_lookups[$parent] = FALSE;
      return $child_lookups[$parent];
    }
  }

  $child_lookups[$parent] = NULL;
  return $child_lookups[$parent];
}

/**
 * Function _themekey_match_condition().
 */
function _themekey_match_condition($condition, &$parameters) {
  if (is_array($condition) && count($condition)) {
    // Default operator is 'equal'
    if (empty($condition['operator'])) {
      $condition['operator'] = '=';
    }

    $value = _themekey_property_value($parameters, $condition['property']);

    if (!is_array($value)) {
      $value = array($value);
    }

    if (!empty($value)) {
      foreach ($value as $single_value) {
        if (!is_null($single_value)) {
          // Supported operators for condition check:
          // smaller ('<'), greater ('>'), equal ('='), not equal ('!'), regex match ('~')
          if ($condition['operator'] == '<' && $single_value >= $condition['value']) {
            return FALSE;
          }
          else if ($condition['operator'] == '>' && $single_value <= $condition['value']) {
            return FALSE;
          }
          else if ($condition['operator'] == '<=' && $single_value > $condition['value']) {
            return FALSE;
          }
          else if ($condition['operator'] == '>=' && $single_value < $condition['value']) {
            return FALSE;
          }
          else if ($condition['operator'] == '=' && $single_value == $condition['value']) {
            return TRUE;
          }
          else if ($condition['operator'] == '!' && $single_value == $condition['value']) {
            return FALSE;
          }
          else if ($condition['operator'] == '~' && preg_match($condition['value'], $single_value)) {
            return TRUE;
          }
        }
        else {
          // value is NULL
          return FALSE;
        }
      }

      // condition matched for all values
      return TRUE;
    }
    else {
      // value array is empty => value is NULL
      return FALSE;
    }
  }
  else {
    trigger_error(t('Function _themekey_match_condition() called with illegal parameters'), E_USER_ERROR);
  }
}

/**
 * Function _themekey_property_value().
 */
function _themekey_property_value(&$parameters, $property) {

  // TODO Warning if property is not part of variable_get('themekey_attributes')

  // Property value is available directly
  if (isset($parameters[$property])) {
    return $parameters[$property];
  }

  $parameters[$property] = NULL;

  $src_candidates = array();
  $maps = variable_get('themekey_maps', array());

  foreach ($maps as $pos => $map) {
    if ($map['dst'] == $property) {
      if (!empty($parameters[$map['src']])) {
        $map_func = $map['callback'];
        $parameters[$property] = $map_func($parameters[$map['src']], $parameters);
        break;
      }
      $src_candidates[$pos] = $map['src'];
    }
  }

  if (is_null($parameters[$property]) && !empty($src_candidates)) {
    foreach ($src_candidates as $pos => $src) {
      $return = _themekey_property_value($parameters, $src);
      if ($return) {
        $map_func = $maps[$pos]['callback'];
        $parameters[$property] = $map_func($return, $parameters);
        break;
      }
    }
  }

  return $parameters[$property];
}


function _themekey_check_theme_enabled($theme, $settings_page = FALSE) {
  static $themes_enabled = array();
  static $warned = FALSE;
  static $displayed_error = FALSE;

  if (!$theme || 'default' == $theme) {
    return TRUE;
  }

  if (empty($themes_enabled)) {
    if ($result = db_query("SELECT name FROM {system} WHERE type = 'theme' AND status = 1;")) {
      while ($row = db_fetch_array($result)) {
        $themes_enabled[] = $row['name'];
      }
    }
  }

  if (in_array($theme, $themes_enabled)) {
    return TRUE;
  }

  if ($settings_page) {
    if (!$displayed_error) {
      drupal_set_message(t("Your current configuration of theme rules uses at least one theme that is not enabled. Nevertheless this configuration is stored but affected rules won't be applied until the targeted theme will be enabled at !build_themes.",
        array('!build_themes' => l('admin/build/themes', 'admin/build/themes'))), 'error');
      $displayed_error = TRUE;
    }
  }
  else {
    global $user;

    if (!$warned && 1 == $user->uid) {
      drupal_set_message(t('A matching Theme Switching Rule to select theme %theme was not applied because this theme is disabled. You can enable this theme at !build_themes or remove this Theme Switching Rule at !themekey_properties or edit current node if the theme was selected using ThemeKey UI.',
        array('%theme' => $theme, '!build_themes' => l('admin/build/themes', 'admin/build/themes'), '!themekey_properties' => l('admin/settings/themekey/properties', 'admin/settings/themekey/properties'))), 'warning');
      $warned = TRUE;
    }
  }

  return FALSE;
}