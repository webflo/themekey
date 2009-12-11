<?php
// $Id$

function themekey_locale_themekey_properties() {
  // Attributes for properties
  $attributes = array();
  $attributes['locale:language'] = array(
    'description' => t('Locale: Language'),
    'validator' => 'themekey_validator_language',
  );

  return array('attributes' => $attributes);
}

function themekey_locale_themekey_global() {
  global $language;

  $parameters = array();
  $parameters['locale:language'] = !empty($language->language) ? $language->language : NULL;

  return $parameters;
}
