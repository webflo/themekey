<?php
// $Id$

/* Path-based */

/**
 * Function _themekey_get_path_ancestors().
 * (based on menu_get_ancestors() in includes/menu.inc)
 */
function _themekey_get_path_ancestors($parts) {
  $placeholders = array();
  $ancestors = array();
  
  $number_parts = count($parts);
  $length =  $number_parts - 1;
  $end = (1 << $number_parts) - 1;
  
  $masks = variable_get('menu_masks', array());
  foreach ($masks as $i) {
    if ($i > $end) {
      continue;
    }
    elseif ($i < (1 << $length)) {
      --$length;
    }
    $current = '';
    for ($j = $length; $j >= 0; $j--) {
      if ($i & (1 << $j)) {
        $current .= $parts[$length - $j];
      }
      else {
        $current .= is_numeric($parts[$length - $j]) ? '#' : '%';
      }
      if ($j) {
        $current .= '/';
      }
    }
    $placeholders[] = "'%s'";
    $ancestors[] = $current;
    if (stristr($current, '#') !== FALSE) {
      $placeholders[] = "'%s'";
      $ancestors[] = str_replace('#', '%', $current);
    }
  }
  
  return array($ancestors, $placeholders);
}

/**
 * Function _themekey_match_paths().
 */
function _themekey_match_paths($path) {
  $parts = explode('/', $path, MENU_MAX_PARTS);
  list($ancestors, $placeholders) = _themekey_get_path_ancestors($parts);
  //
  $result = db_query('SELECT * FROM {themekey_paths} WHERE path IN ('. implode (',', $placeholders) .') ORDER BY fit DESC, weight DESC, custom DESC', $ancestors);
  while ($item = db_fetch_array($result)) {
    $parameters = array();
    $conditions = unserialize($item['conditions']);
    $wildcards = unserialize($item['wildcards']);
    foreach ($wildcards as $index => $wildcard) {
      $parameters[$wildcard] = arg($index, $path);
    }
    //
    if (!_themekey_match_conditions($conditions, $parameters)) {
      continue;
    }
    //
    $callbacks = unserialize($item['callbacks']);
    if (count($callbacks)) {
      foreach ($callbacks as $callback) {
        $callback($item, $parameters);
      }
    }
    //
    if (!$item['theme']) {
      $item['theme'] = _themekey_match_properties($parameters);
    }

    return $item['theme'];
  }

  return NULL;
}

/**
 * Function _themekey_match_conditions().
 */
function _themekey_match_conditions($conditions, $parameters) {
  if (count($conditions)) {
    foreach ($conditions as $condition) {
      list ($key, $value) = explode('=', $condition);
      if (isset($parameters[$key]) && $parameters[$key] != $value) {
        return FALSE;
      }
    }
  }

  return TRUE;
}

/* Property-based */

/**
 * Function _themekey_match_properties().
 */
function _themekey_match_properties($object) {
  $properties = variable_get('themekey_properties', array());
  foreach ($properties as $property => $details) {
    $value = _themekey_property_field($object, $property, $details['path']);
    if (!empty($value) || count($value)) {
      $query = 'SELECT * FROM {themekey_properties} WHERE property = \''. $property .'\' AND value ';
      $query .= is_array($value) ? ('IN ('. db_placeholders($value, 'varchar') .') ORDER BY weight') : '= \'%s\'';
      $result = db_query($query, $value);
      while ($item = db_fetch_array($result)) {
        //
        $item['conditions'] = unserialize($item['conditions']);
        if (count($item['conditions'])) {
          foreach ($item['conditions'] as $condition) {
            if (isset($properties[$condition['property']]['path'])) {
              $value = _themekey_property_field($object, $condition['property'], $properties[$condition['property']]['path']);
              $value = is_array($value) ? $value : array($value);
              if (in_array($condition['value'], $value)) {
                return NULL;
              }
            }
          }
        }
        //
        $callbacks = unserialize($item['callbacks']);
        if (count($callbacks)) {
          foreach ($callbacks as $callback) {
            $callback($item, $parameters);
          }
        }
        //
        return $item['theme'];
      }
    }
  }
  
  return NULL;
}

/**
 * Function _themekey_property_field().
 */
function _themekey_property_field($object, $property, $path) {
  $value = (array)$object;
  //
  $parts = isset($value[$property]) ? array($property) : explode('/', $path);
  foreach ($parts as $part) {
    $value = (array)$value;
    if (is_array($value) && isset($value[$part])) {
      $value = $value[$part];
    }
    else {
      // Find mapped properties
      $value = (array)$object;
      $nidmap = array('src' => FALSE, 'dst' => FALSE);
      $maps = variable_get('themekey_maps', array());
      foreach ($maps as $pos => $map) {
        if (isset($value[$map['src']]) && $map['dst'] == 'nid') {
          $nidmap['src'] = $pos;
        }
        if ($map['src'] == 'nid' && $map['dst'] == $property) {
          $nidmap['dst'] = $pos;
        }
        if (isset($value[$map['src']]) && $map['dst'] == $property) {
          $map_func = $map['callback'];
          return $map_func($value[$map['src']], $object);
        }
      }
      // Support mapping via node id (src -> nid -> dst)
      if ($nidmap['src'] !== FALSE && $nidmap['dst'] !== FALSE) {
        $src_func = $maps[$nidmap['src']]['callback'];
        $dst_func = $maps[$nidmap['dst']]['callback'];
        return $dst_func($src_func($value[$maps[$nidmap['src']]['src']], $object), $object);
      }

      return NULL;
    }
  }

  return $value;
}
