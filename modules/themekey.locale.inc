<?php
// $Id$

function themekey_locale_themekey_properties() {
  // Attributes for properties
  $attributes = array();
  $attributes['locale:language'] = array(
    'description' => t('Locale: Language')
  );

  return array('attributes' => $attributes);
}

function themekey_locale_themekey_global() {
  global $language;

  $parameters = array();
  // REVIEW set default language instead of 'en'
  $parameters['locale:language'] = isset($language->language) ? $language->language : 'en';

  return $parameters;
}