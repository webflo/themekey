<?php
// $Id$

function themekey_system_themekey_properties() {
  // Attributes for properties
  $attributes = array();
  $attributes['system:host'] = array(
    'description' => t('System: Server Host')
  );
  $attributes['drupal:path'] = array(
    'description' => t('Drupal: Drupal path or path alias')
  );

  return array('attributes' => $attributes);
}

function themekey_system_themekey_global() {
  global $user;

  $parameters = array();
  $parameters['system:host'] = $_SERVER['HTTP_HOST'];

  list($ancestors, $placeholders) = _themekey_get_path_ancestors($_GET['q']);
  foreach ($ancestors as $ancestor) {
    $parameters['drupal:path'][] = $ancestor;
  }

  if (module_exists('path')) {
    // Derive path from request_uri
    $offset = (variable_get('clean_url', 0) ? 0 : 3) + strlen(base_path());
    $alias_uri = substr(request_uri(), $offset);
    if (strpos($alias_uri, '?') !== FALSE) {
      // Remove query string from request uri
      $alias_uri_parts = explode('?', $alias_uri);
      $alias_uri = $alias_uri_parts[0];
    }
    // For $alias_uri != $_GET['q'] the page was requested using an
    // aliased path, otherwise get the path alias internally
    if ($alias_uri == $_GET['q']) {
      $alias_uri = drupal_get_path_alias($_GET['q']);
    }

    if ($alias_uri != $_GET['q']) {
      $parameters['internal:alias_uri'] = $alias_uri;

      list($ancestors, $placeholders) = _themekey_get_path_ancestors($alias_uri);
      foreach ($ancestors as $ancestor) {
        $parameters['drupal:path'][] = $parameters['internal:path_alias'][] = $ancestor;
      }
    }
  }

  return $parameters;
}
