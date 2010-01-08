<?php
// $Id$

/**
 * @file
 */


/**
 * Uses drupal's form builder to format ThemeKey's help tutorials
 *
 * @see themekey_help()
 */
function themekey_help_tutorials_form($collapsed = TRUE) {
  $form['themekey_help_tutorials'] = array(
    '#type' => 'fieldset',
    '#title' => t('Tutorials'),
    '#collapsible' => TRUE,
    '#collapsed' => $collapsed,
  );

  $form['themekey_help_tutorials']['Rule Chaining'] = array(
    '#type' => 'fieldset',
    '#title' => t('Rule Chaining'),
    '#collapsible' => TRUE,
    '#collapsed' => $form['themekey_help_tutorials']['#collapsed'],
  );

  $img_path = base_path() . drupal_get_path('module', 'themekey') . '/img/tutorials/';

  $form['themekey_help_tutorials']['Rule Chaining']['item'] = array(
    '#type' => 'item',
    '#value' =>
'<p>' . t('Note: This tutorial uses some ThemeKey properties from an additional module called !themekey_properties_link.', array('!themekey_properties_link' => l('ThemeKey Properties', 'http://drupal.org/project/themekey_properties'))) . '</p>' .
'<p>' . t("In this tutorial you'll learn how to define ThemeKey rules and how to cascade them to achieve sophisticated rule chains.") . '</p>' .
'<img src="' . $img_path . 'create_content.png" />' .
'<p>' . t('Have Fun!') . '</p>',
    );

  return $form;
}


/**
 * Uses drupal's form builder to format ThemeKey's help examples
 *
 * @see themekey_help()
 */
function themekey_help_examples_form($collapsed = TRUE) {
  $form['themekey_help_examples'] = array(
    '#type' => 'fieldset',
    '#title' => t('Examples'),
    '#collapsible' => TRUE,
    '#collapsed' => $collapsed,
  );

  $form['themekey_help_examples']['Set a special theme for site administrator'] = array(
    '#type' => 'fieldset',
    '#title' => t('Set a special theme for site administrator'),
    '#collapsible' => TRUE,
    '#collapsed' => $form['themekey_help_examples']['#collapsed'],
  );

  $form['themekey_help_examples']['Set a special theme for site administrator']['item'] = array(
    '#type' => 'item',
    '#value' => t('Create a Theme Switching Rules:') . '<pre>
Property: user:uid
Operator: =
Value: 1
</pre>',
    );

  // TODO: add some examples using drupal:path and drupal:path:wildcard
  $form['themekey_help_examples']['TODO: add some examples using drupal:path and drupal:path:wildcard'] = array(
    '#type' => 'fieldset',
    '#title' => 'TODO: add some examples using drupal:path and drupal:path:wildcard',
    '#collapsible' => TRUE,
    '#collapsed' => $form['themekey_help_examples']['#collapsed'],
  );

  $form['themekey_help_examples']['TODO: add some examples using drupal:path and drupal:path:wildcard']['item'] = array(
    '#type' => 'item',
    '#value' => 'TODO' . '<pre>
</pre>',
    );

  return $form;
}


/**
 * Uses drupal's form builder to format ThemeKey's help properties
 *
 * @see themekey_help()
 */
function themekey_help_properties_form($collapsed = TRUE) {
  $attributes = variable_get('themekey_attributes', array());

  $form['themekey_help_properties'] = array(
    '#type' => 'fieldset',
    '#title' => t('Properties explained'),
    '#collapsible' => TRUE,
    '#collapsed' => $collapsed,
  );

  foreach ($attributes as $property => $attribute) {
    $form['themekey_help_properties'][$property] = array(
      '#type' => 'fieldset',
      '#title' => $property,
      '#collapsible' => TRUE,
      '#collapsed' => $collapsed,
    );

    $form['themekey_help_properties'][$property]['item'] = array(
      '#type' => 'item',
      '#value' => $attribute['description'],
    );
  }

  return $form;
}
