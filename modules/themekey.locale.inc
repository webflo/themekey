<?php
// $Id$

function themekey_locale_themekey_properties() {
  // Attributes for properties
  $attributes = array();
  $attributes['locale:language'] = array(
    'description' => t('Locale: Language - The code of the current site language, formatted like "en" or "de". See !link for your the codes of the enabled languages', 
      array('!link' => l('admin/settings/language', 'admin/settings/language'))),
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
