<?php
// $Id$

function themekey_system_themekey_properties() {
  // Attributes for properties
  $attributes = array();
  $attributes['system:host'] = array(
    'description' => t('System: Server Host')
  );

  return array('attributes' => $attributes);
}

function themekey_system_themekey_global() {
  global $user;

  $parameters = array();
  $parameters['system:host'] = $_SERVER['HTTP_HOST'];

  return $parameters;
}
