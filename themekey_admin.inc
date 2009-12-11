<?php
// $Id$

require_once(drupal_get_path('module', 'themekey') .'/themekey_build.inc');

define('THEMEKEY_PAGER_LENGTH', 40);

/**
 * Function _themekey_properties_form().
 */
function themekey_properties_form() {

  _themekey_admin_theme_warning();

  $properties = variable_get('themekey_properties', array());

  if (empty($properties)) {
    drupal_goto('admin/settings/themekey/settings');
  }

  $themes = _themekey_theme_options();
  $attributes = variable_get('themekey_attributes', array());
  $operators = array(
    '=' => '=',
    '!' => '!',
    '<' => '<',
    '<=' => '<=',
    '>' => '>',
    '>=' => '>=',
    '~' => '~',
  );

  $form = array('#tree' => TRUE);

  $form['help'] = array(
    '#type' => 'fieldset',
    '#title' => t('Help'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['help']['description_1'] = array(
    '#type' => 'item',
    '#title' => t('How to define Theme Switching Rules'),
    '#value' => t(
'Text copied from ThemeKey 6.x-1.2 properties description and needs to be reviewed:
Here you can map themes to object properties, e.g. to node properties such as the NodeID (nid).
To add a new map entry to the table, select a property from the "Property" dropdown and enter
the value which you want to assign the theme to. For example: Property = "nid", Value = "123"
and Theme = "Bluemarine" switches the theme of node 123 to Bluemarine. You may also specify
"Additional Conditions" so that the theme only applies when all conditions are matched. "Additional
Conditions" must be provided using the following syntax: "property=value;property2=value2;...".
Supported operators are "=" (equal), "!" (not equal), "<" (smaller), ">" (greater) and "~" (regex match).'),
  );

  $form['help']['properties'] = array(
    '#type' => 'fieldset',
    '#title' => t('Properties explained'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  foreach ($properties as $property) {
    $form['help']['properties'][$property] = array(
      '#type' => 'item',
      '#title' => $property,
      '#value' => $attributes[$property]['description'],
    );
  }

  $items = _themekey_load_properties(THEMEKEY_PAGER_LENGTH);
  $parent_options = array_merge(array(0), array_keys($items));
  $parent_options = array_combine($parent_options, $parent_options);
  foreach ($items as $item) {
    $property = $item['property'];
    $wildcard = '';
    if (!in_array($property, $properties)) {
      $property = 'drupal:path:wildcard';
      $wildcard = $item['property'];
    }

    $form['old_items'][$item['id']]['depth'] = array(
      '#type' => 'value',
      '#value' => $item['depth'],
    );
    $form['old_items'][$item['id']]['id'] = array(
      '#type' => 'hidden',
      '#value' => $item['id'],
    );
    $form['old_items'][$item['id']]['property'] = array(
      '#type' => 'select',
      '#default_value' => $property,
      '#options' => $properties,
    );
    $form['old_items'][$item['id']]['wildcard'] = array(
      '#type' => 'textfield',
      '#default_value' => $wildcard,
      '#size' => 10,
      '#maxlength' => 255,
    );
    $form['old_items'][$item['id']]['operator'] = array(
      '#type' => 'select',
      '#default_value' => $item['operator'],
      '#options' => $operators,
    );
    $form['old_items'][$item['id']]['value'] = array(
      '#type' => 'textfield',
      '#default_value' => $item['value'],
      '#size' => 20,
      '#maxlength' => 255,
    );
    $form['old_items'][$item['id']]['parent'] = array(
      '#type' => 'select',
      '#default_value' => $item['parent'],
      '#options' => $parent_options
    );
    $form['old_items'][$item['id']]['theme'] = array(
      '#type' => 'select',
      '#default_value' => $item['theme'],
      '#options' => $themes,
    );
    $form['old_items'][$item['id']]['enabled'] = array(
      '#type' => 'checkbox',
      '#default_value' => $item['enabled'],
    );
    $form['old_items'][$item['id']]['weight'] = array(
      '#type' => 'weight',
      '#delta' => 50,
      '#default_value' => $item['weight'],
    );
    $form['old_items'][$item['id']]['delete'] = array(
      '#value' => l(t('delete'), 'admin/settings/themekey/properties/delete/' . $item['id']),
    );
  }

  if (count($items)) {
    $form['pager'] = array('#value' => theme('pager', array(), THEMEKEY_PAGER_LENGTH));
  }

  $form['new_item']['property'] = array(
    '#type' => 'select',
    '#default_value' => !empty($_GET['property']) ? check_plain($_GET['property']) : '',
    '#options' => $properties,
  );
  $form['new_item']['wildcard'] = array(
    '#type' => 'textfield',
    '#default_value' => $wildcard,
    '#size' => 10,
    '#maxlength' => 255,
  );
  $form['new_item']['operator'] = array(
    '#type' => 'select',
    '#default_value' => '=',
    '#options' => $operators,
  );
  $form['new_item']['value'] = array(
    '#type' => 'textfield',
    '#default_value' => !empty($_GET['value']) ? check_plain($_GET['value']) : '',
    '#size' => 25,
    '#maxlength' => 255
  );
  $form['new_item']['theme'] = array(
    '#type' => 'select',
    '#default_value' => 'default',
    '#options' => $themes,
  );
  $form['new_item']['enabled'] = array(
    '#type' => 'checkbox',
    '#default_value' => TRUE,
  );

  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

// validate after submit and disable rule in database. otherwise all drag and drop stuff is lost in case of an error
function themekey_properties_form_alter(&$form, $form_state, $form_id) {
  $attributes = variable_get('themekey_attributes', array());

  if ('themekey_properties_form' == $form_id && empty($form_state['post'])) {
    require_once(drupal_get_path('module', 'themekey') .'/themekey_validators.inc');

    if (!empty($form['old_items'])) {
      foreach ($form['old_items'] as $key => $value) {
        if ($value['enabled']['#default_value']) {
          if (!_themekey_check_theme_enabled($value['theme']['#default_value'], TRUE)) {
            themekey_properties_form_set_error($form, $key, 'theme', t('Theme is not activated'));
          }

          if (0 == strlen($value['value']['#default_value'])) {
            themekey_properties_form_set_error($form, $key, 'value', t('You must enter a value'));
          }
          else if (!empty($attributes[$value['property']['#default_value']]['validator'])) {
            //validate rule with custom validator
            $validator = $attributes[$value['property']['#default_value']]['validator'];
            if (function_exists($validator)) {
              $rule = array(
                'property' => $value['property']['#default_value'],
                'wildcard' => $value['wildcard']['#default_value'],
                'operator' => $value['operator']['#default_value'],
                'value' => $value['value']['#default_value'],
              );
              $errors = $validator($rule);
              foreach ($errors as $element => $msg) {
                themekey_properties_form_set_error($form, $key, $element, $msg);
              }
            }
            else {
              themekey_properties_form_set_error($form, $key, 'property', t('ThemeKey requested an unknown validator called %validator to validate property %property', array('%validator' => $validator, '%property' => $value['property']['#default_value'])));
            }
          }

          foreach ($form['old_items'] as $key_2 => $value_2) {
            if ($key_2 == $key) {
              continue;
            }

            if ($value_2['enabled']['#default_value'] &&
                !empty($value['value']['#default_value']) &&
                $value_2['property']['#default_value'] == $value['property']['#default_value'] &&
                $value_2['operator']['#default_value'] == $value['operator']['#default_value'] &&
                $value_2['value']['#default_value'] == $value['value']['#default_value'] &&
                $value_2['parent']['#default_value'] == $value['parent']['#default_value']) {

              if ('drupal:path:wildcard' != $value_2['property']['#default_value'] ||
                  ('drupal:path:wildcard' == $value_2['property']['#default_value'] && $value_2['wildcard']['#default_value'] == $value['wildcard']['#default_value'])) {
                themekey_properties_form_set_error($form, $key, 'property', t('You entered two identical theme switching rules in the chain'));
              }
            }
          }
        }
      }
    }

    // dump all errors
    themekey_properties_form_set_error($form);
  }
}

function themekey_properties_form_set_error(&$form, $rule_id = 0, $element = '', $msg = '') {
  static $error_stack = array();

  if (!empty($rule_id)) {
    $error_stack[$rule_id][] = array('old_items][' . $rule_id . '][' . $element, $msg);
  }
  else {
    $error_counter = 0;
    foreach ($error_stack as $rule_id => $errors) {
      $error_counter++;
      $form['old_items'][$rule_id]['id']['#prefix'] = '<span class="error">' . $error_counter . '</span>';
      foreach ($errors as $error) {
        form_set_error($error[0], $error_counter .': ' . $error[1]);
      }
    }
  }
}

/**
 * Function themekey_properties_submit().
 */
function themekey_properties_form_submit($form, &$form_state) {
  $max_weight = 0;
  if (!empty($form_state['values']['old_items'])) {
    foreach ($form_state['values']['old_items'] as $id => $item) {
      if ($item['weight'] > $max_weight) {
        $max_weight = $item['weight'];
      }

      $item['id'] = $id;

      if ('drupal:path:wildcard' == $item['property']) {
        $item['property'] = $item['wildcard'];
      }

      unset($item['wildcard']);

      _themekey_properties_set($item);
    }
  }

  if (!empty($form_state['values']['new_item']['value'])) {
    $item = $form_state['values']['new_item'];

    $item['parent'] = 0;
    $item['weight'] = $max_weight + 1;

    if ('drupal:path:wildcard' == $item['property']) {
      $item['property'] = $item['wildcard'];
    }

    unset($item['wildcard']);

    _themekey_properties_set($item);
  }

  drupal_set_message(t('The configuration options have been saved.'));
}

/**
 * Function _themekey_settings_form().
 */
function _themekey_settings_form() {
  // REVIEW rebuild is called to many times
  // REVIEW isn't there a better place for this?
  _themekey_rebuild();

  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('General Settings'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE
  );

  $form['settings']['themekey_allthemes'] = array(
    '#type' => 'checkbox',
    '#title' => t('Provide all themes for selection'),
    '#default_value' => variable_get('themekey_allthemes', 0),
    '#description' => t('Make all installed themes available for selection, not enabled ones only.')
  );

  $form['settings']['themekey_theme_maintain'] = array(
    '#type' => 'checkbox',
    '#title' => t('Retain the theme until a new theme is set'),
    '#default_value' => variable_get('themekey_theme_maintain', 0),
    '#description' => t('Select this option to have users logged in stay in the same theme until they browse to a new page with an explicit theme set.'),
  );

  $form['settings']['themekey_theme_maintain_anonymous'] = array(
    '#type' => 'checkbox',
    '#title' => t('Retain the theme until a new theme is set for anonymous users'),
    '#default_value' => variable_get('themekey_theme_maintain', 0),
    '#description' => t('Select this option to have anonymous users stay in the same theme until they browse to a new page with an explicit theme set.
<br /><b>Warning: This feature is only working correctly if you turn off caching!</b>'),
  );

  if (module_exists('forum')) {
    $form['settings']['themekey_module_forum_triggers_taxonomy_vid'] = array(
      '#type' => 'checkbox',
      '#title' => t('Forum pages trigger property taxonomy:vid'),
      '#default_value' => variable_get('themekey_module_forum_triggers_taxonomy_vid', 0),
      '#description' => t('Property taxonomy:vid is set when a single node is shown (p.e. /node/17). If this option is selected, forum pages like /forum/28 will set taxonomy:vid as well.')
    );
  }

// TODO Integration of Taxonomy Menu outdated. See http://drupal.org/node/616946
//  if (module_exists('taxonomy_menu')) {
//    $form['settings']['themekey_module_taxonomy_menu_triggers_taxonomy_tid'] = array(
//      '#type' => 'checkbox',
//      '#title' => t('Module <em>Taxonomy Menu</em> triggers property taxonomy:tid'),
//      '#default_value' => variable_get('themekey_module_taxonomy_menu_triggers_taxonomy_tid', 0),
//      '#description' => t('Property taxonomy:tid is set when a single node is shown (p.e. /node/17). If this option is selected, pages created by module Taxonomy Menu like /category/31 will set taxonomy:tid as well.')
//    );
//  }

  return system_settings_form($form);
}

function _themekey_settings_form_validate($form, &$form_state) {
}

/**
 * Theme the menu overview form into a table.
 *
 * @ingroup themeable
 */
function theme_themekey_properties_form($form) {
  $output = '';

  $rows = array();

  if (!empty($form['old_items'])) {
    $num_childs = array();
    $parents_disabled = array();

    foreach ($form['old_items'] as $key => $item) {
      if (isset($item['property'])) {
        $num_childs[$key] = 0;
        $parents_disabled[$key] = FALSE;

        if (!empty($parents_disabled[$item['parent']['#value']])) {
          $form['old_items'][$key]['enabled']['#value'] = 0;
        }

        if (!$form['old_items'][$key]['enabled']['#value']) {
          $parents_disabled[$key] = TRUE;
        }
        else if ($item['parent']['#value']) {
          $num_childs[$item['parent']['#value']]++;
        }
      }
    }

    foreach ($form['old_items'] as $key => $item) {
      if (isset($item['property'])) {
      $row = is_array($form['old_items'][$key]['#attributes']) ? $form['old_items'][$key]['#attributes'] : array();

        // Add special classes to be used for tabledrag.js.
        $form['old_items'][$key]['id']['#attributes']['class'] = 'themekey-property-id';
        $form['old_items'][$key]['parent']['#attributes']['class'] = 'themekey-property-parent';
        $form['old_items'][$key]['weight']['#attributes']['class'] = 'themekey-property-weight';
        // Add special classes to be used for themekey-properties.js.
        $form['old_items'][$key]['property']['#attributes']['class'] = 'themekey-property-property themekey-fadeable';
        $form['old_items'][$key]['wildcard']['#attributes']['class'] = 'themekey-property-wildcard themekey-fadeable';
        $form['old_items'][$key]['operator']['#attributes']['class'] = 'themekey-fadeable';
        $form['old_items'][$key]['value']['#attributes']['class'] = 'themekey-fadeable';
        $form['old_items'][$key]['enabled']['#attributes']['class'] = 'themekey-property-enabled';
        $form['old_items'][$key]['theme']['#attributes']['class'] = 'themekey-property-theme themekey-fadeable';
        if ('drupal:path:wildcard' != $item['property']['#value']) {
          $form['old_items'][$key]['wildcard']['#attributes']['style'] = 'display: none';
        }
        if ($num_childs[$key]) {
          $form['old_items'][$key]['theme']['#attributes']['style'] = 'display: none';
        }
        if ($parents_disabled[$item['parent']['#value']]) {
          $form['old_items'][$key]['enabled']['#attributes']['style'] = 'display: none';
          $form['old_items'][$key]['enabled']['#default_value'] = 0;
        }

        $elements = array();

        $elements[] = array('data' =>
          theme('indentation', $form['old_items'][$key]['depth']['#value']) .
          drupal_render($form['old_items'][$key]['id']) .
          drupal_render($form['old_items'][$key]['property']) .
          drupal_render($form['old_items'][$key]['wildcard']) .
          drupal_render($form['old_items'][$key]['operator']) .
          drupal_render($form['old_items'][$key]['value']),
          'class' => 'themekey-properties-row');

        $elements[] = array('data' =>
          drupal_render($form['old_items'][$key]['theme']));

        $elements[] = array('data' =>
          drupal_render($form['old_items'][$key]['enabled']));

        $elements[] = array('data' =>
          drupal_render($form['old_items'][$key]['delete']));

        $elements[] = array('data' =>
          drupal_render($form['old_items'][$key]['parent']));

        $elements[] = array('data' =>
          drupal_render($form['old_items'][$key]['weight']));

        $row['class'] = !empty($row['class']) ? $row['class'] .' draggable' : 'draggable';
        if (!$form['old_items'][$key]['enabled']['#value']) {
          $row['class'] .= ' themekey-fade-out';
        }
        if (!$form['old_items'][$key]['parent']['#value']) {
          $row['class'] .= ' themekey-top-level';
        }
        $row['id'] = 'themekey-properties-row-' . $key;
        $row['data'] = $elements;
        $rows[] = $row;
      }
    }
  }

  if (!empty($rows)) {
    if (empty($form['pager']['#value'])) {
      drupal_add_tabledrag('themekey-properties', 'match', 'parent', 'themekey-property-parent', 'themekey-property-parent', 'themekey-property-id', TRUE);
      drupal_add_tabledrag('themekey-properties', 'order', 'sibling', 'themekey-property-weight');
    }

    $header = array(
      array('data' => t('Theme Switching Rule Chain')),
      array('data' => t('Theme')),
      array('data' => t('Enabled')),
      array('data' => t('Operation')),
      array('data' => t('Parent')),
      array('data' => t('Weight')),
    );

    $output .= theme('table', $header, $rows, array('id' => 'themekey-properties'));

    foreach ($num_childs as $parent => $num) {
      $output .= '<input id="themekey-num-childs-' . $parent . '" type="hidden" value="' . $num . '"></input>';
    }
  }

  $rows = array();

  if (!empty($form['new_item'])) {
    if (isset($form['new_item']['property'])) {
      $row = is_array($form['new_item']['#attributes']) ? $form['new_item']['#attributes'] : array();

      // Add special classes to be used for themekey-properties.js.
      $form['new_item']['property']['#attributes']['class'] = 'themekey-property-property';
      $form['new_item']['wildcard']['#attributes']['class'] = 'themekey-property-wildcard';
      if ('drupal:path:wildcard' != $form['new_item']['property']['#value']) {
        $form['new_item']['wildcard']['#attributes']['style'] = 'display: none';
      }

      $elements = array();
      $elements[] = t('New Rule:');
      $elements[] = array('data' =>
        drupal_render($form['new_item']['property']) .
        drupal_render($form['new_item']['wildcard']) .
        drupal_render($form['new_item']['operator']) .
        drupal_render($form['new_item']['value']),
        'class' => 'themekey-properties-row');
      $elements[] = array('data' => drupal_render($form['new_item']['theme']));
      $elements[] = array('data' => drupal_render($form['new_item']['enabled']));

      $row['data'] = $elements;
      $rows[] = $row;
    }
  }

  if (!empty($rows)) {
    $header = array(
      '',
      array('data' => t('Theme Switching Rule')),
      array('data' => t('Theme')),
      array('data' => t('Enabled')),
    );

    $output .= '<a name="themekey_new_rule"></a>';
    $output .= theme('table', $header, $rows, array('id' => 'themekey-new-item'));
  }

  $output .= drupal_render($form);

  drupal_add_css(drupal_get_path('module', 'themekey') . '/themekey_properties.css');
  drupal_add_js(drupal_get_path('module', 'themekey') . '/themekey_properties.js');

  return $output;
}

function _themekey_admin_theme_warning() {
  if (variable_get('admin_theme', '0')) {
    drupal_set_message(t('Note: %admin_theme is configured as administration theme at !link. This setting is more powerful than a corresponding ThemeKey rule.',
      array('%admin_theme' => variable_get('admin_theme', '0'), '!link' => l('admin/settings/admin', 'admin/settings/admin'))), 'warning');
    if (variable_get('node_admin_theme', '0')) {
      drupal_set_message(t('Note: As configured at !link adding or editing a node will use the administration theme %admin_theme.',
        array('%admin_theme' => variable_get('admin_theme', '0'), '!link' => l('admin/settings/admin', 'admin/settings/admin'))), 'warning');
    }
  }
}

/**
 * Menu callback -- ask for confirmation of themekey property deletion
 */
function themekey_admin_delete_property_confirm(&$form_state, $arg, $themekey_property_id) {
  $form['themekey_property_id'] = array(
    '#type' => 'value',
    '#value' => $themekey_property_id,
  );

  return confirm_form($form,
    t('Are you sure you want to delete %title?', array('%title' => $themekey_property_id)),
    'admin/settings/themekey/properties',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Execute themekey property deletion
 */
function themekey_admin_delete_property_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    _themekey_properties_del($form_state['values']['themekey_property_id']);
  }

  $form_state['redirect'] = 'admin/settings/themekey/properties';
}
