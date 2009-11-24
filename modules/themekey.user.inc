<?php
// $Id$

function themekey_user_themekey_properties() {
  // Attributes for properties
  $attributes = array();
  $attributes['user:hostname'] = array(
    'description' => t('User: Hostname')
  );
  $attributes['user:language'] = array(
    'description' => t('User: Language')
  );
  $attributes['user:name'] = array(
    'description' => t('User: Name')
  );
  $attributes['user:uid'] = array(
    'description' => t('User: ID')
  );

  return array('attributes' => $attributes);
}

function themekey_user_themekey_global() {
  global $user;

  $parameters = array();
  $parameters['user:hostname'] = !empty($user->hostname) ? $user->hostname : NULL;
  $parameters['user:language'] = !empty($user->language) ? $user->language : NULL;
  $parameters['user:name'] = !empty($user->name) ? $user->name : NULL;
  $parameters['user:uid'] = $user->uid;

  return $parameters;
}
