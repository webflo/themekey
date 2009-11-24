<?php
// $Id$

require_once(drupal_get_path('module', 'themekey') .'/themekey_build.inc');

define('THEMEKEY_PAGER_LENGTH', 40);

/**
 * Function _themekey_properties_form().
 */
function themekey_properties_form() {

  _themekey_admin_theme_warning();

  $form = array('#tree' => TRUE);

  $form['help'] = array(
    '#type' => 'fieldset',
    '#title' => t('Help'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t(
'Here you can map themes to object properties, e.g. to node properties such as the NodeID (nid).
To add a new map entry to the table, select a property from the \'Property\' dropdown and enter
the value which you want to assign the theme to. For example: Property = \'nid\', Value = \'123\'
and Theme = \'Bluemarine\' switches the theme of node 123 to Bluemarine. You may also specify
\'Additional Conditions\' so that the theme only applies when all conditions are matched. \'Additional
Conditions\' must be provided using the following syntax: \'property=value;property2=value2;...\'.
Supported operators are \'=\' (equal), \'!\' (not equal), \'<\' (smaller), \'>\' (greater) and \'~\' (regex match).') .
t(
'Here you can map themes to Drupal paths or path aliases. To add a new map entry to the table,
enter the path into the \'Path\' field and select a theme from the \'Theme\' dropdown. For example:
Path = \'node/add\' and Theme = \'Bluemarine\' switches the theme of the \'Create content\' page to
Bluemarine. You may also use wildcard characters in the path and/or specify \'Conditions\' to match.<br />
Wildcard characters are \'#\' for numeric parts and \'%\' for all characters. To match conditions
against a certain part, use an identifier with the wildcard. For example: Path =
\'comment/reply/#xyz\' matches all paths with \'comment/reply\' and a numeric third argument.
You can then specify conditions for every wildcard argument using the following syntax:
\'id=value;id2=value2;...\', e.g. \'xyz=123\'. Supported operators are \'=\' (equal), \'!\' (not equal),
\'<\' (smaller), \'>\' (greater) and \'~\' (regex match).'),
  );

  $themes = _themekey_theme_options();
  $properties = variable_get('themekey_properties', array());

  $items = _themekey_load_properties(THEMEKEY_PAGER_LENGTH, FALSE);
  foreach ($items as $item) {
    $form['old_items'][$item['id']]['property'] = array(
      '#type' => 'select',
      '#default_value' => $item['property'],
      '#options' => $properties,
    );
    $form['old_items'][$item['id']]['value'] = array(
      '#type' => 'textfield',
      '#default_value' => $item['value'],
      '#size' => 25,
      '#maxlength' => 255
    );
    $form['old_items'][$item['id']]['conditions'] = array(
      '#type' => 'textfield',
      '#default_value' => $item['conditions'],
      '#size' => 35,
      '#maxlength' => 255
    );
    $form['old_items'][$item['id']]['theme'] = array(
      '#type' => 'select',
      '#default_value' => $item['theme'],
      '#options' => $themes,
    );
    if (!_themekey_check_theme_enabled($item['theme'], TRUE)) {
      $form['old_items'][$item['id']]['theme']['#attributes'] = array('class' => 'error');
    }
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
  $form['new_item']['value'] = array(
    '#type' => 'textfield',
    '#default_value' => !empty($_GET['value']) ? check_plain($_GET['value']) : '',
    '#size' => 25,
    '#maxlength' => 255
  );
  $form['new_item']['conditions'] = array(
    '#type' => 'textfield',
    '#default_value' => '',
    '#size' => 35,
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


function themekey_properties_form_validate($form, &$form_state) {
  if (!empty($form_state['values']['old_items'])) {
    foreach ($form_state['values']['old_items'] as $key => $value) {
      if (empty($value['value'])) {
        form_set_error('old_items][' . $key . '][value', t('You must enter a value'));
        break;
      }

      if (!empty($value['conditions'])) {
        $conditions = _themekey_properties_explode_conditions($value['conditions']);
        if (empty($conditions) || count($conditions) != (substr_count($value['conditions'], ';') + 1)) {
          form_set_error('old_items][' . $key . '][conditions', t('You entered malformatted conditions'));
        }
      }

      foreach ($form_state['values']['old_items'] as $key_2 => $value_2) {
        if ($key_2 == $key) {
          continue;
        }

        if (!empty($value['value']) && $value_2['property'] == $value['property'] && $value_2['value'] == $value['value'] && $value_2['conditions'] == $value['conditions']) {
          form_set_error('old_items][' . $key . '][value', t('You entered two identical values with identical conditions for the same property'));
          break(2);
        }
      }

      if (!empty($value['value']) && $form_state['values']['new_item']['property'] == $value['property'] && $form_state['values']['new_item']['value'] == $value['value'] && $form_state['values']['new_item']['conditions'] == $value['conditions']) {
        form_set_error('new_item][value', t('You entered two identical values with identical conditions for the same property'));
        break;
      }
    }
  }
}


/**
 * Function themekey_properties_submit().
 */
function themekey_properties_form_submit($form, &$form_state) {
  if (!empty($form_state['values']['old_items'])) {
    foreach ($form_state['values']['old_items'] as $id => $item) {
//      if (empty($item['value'])) {
//        _themekey_properties_del($id);
//      }
      $item['id'] = $id;
      _themekey_properties_set($item);
    }
  }

  if (!empty($form_state['values']['new_item']['value'])) {
    _themekey_properties_set($form_state['values']['new_item']);
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
    '#default_value' => variable_get('themekey_allthemes', 1),
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
    foreach ($form['old_items'] as $key => $item) {
      if (isset($item['property'])) {
        $row = is_array($form['old_items'][$key]['#attributes']) ? $form['old_items'][$key]['#attributes'] : array();

        $elements = array();
        $elements[] = '';
        $elements[] = array('data' => drupal_render($form['old_items'][$key]['property']), 'class' => 'select');
        $elements[] = array('data' => drupal_render($form['old_items'][$key]['value']), 'class' => 'textfield');
        $elements[] = array('data' => drupal_render($form['old_items'][$key]['conditions']), 'class' => 'textfield');
        $elements[] = array('data' => drupal_render($form['old_items'][$key]['theme']), 'class' => 'select');
        $elements[] = array('data' => drupal_render($form['old_items'][$key]['enabled']), 'class' => 'checkbox');
        // Add special classes to be used for tabledrag.js.
        $form['old_items'][$key]['weight']['#attributes']['class'] = 'themekey-property-weight';
        $elements[] = array('data' => drupal_render($form['old_items'][$key]['weight']));
        $elements[] = array('data' => drupal_render($form['old_items'][$key]['delete']));

        $row['class'] = !empty($row['class']) ? $row['class'] .' draggable' : 'draggable';
        $row['data'] = $elements;
        $rows[] = $row;
      }
    }
  }

  if (!empty($rows)) {
    if (empty($form['pager']['#value'])) {
      drupal_add_tabledrag('themekey-properties', 'order', 'sibling', 'themekey-property-weight');
    }

    $header = array(
      '',
      array('data' => t('Property'), 'class' => 'select'),
      array('data' => t('Value'), 'class' => 'textfield'),
      array('data' => t('Conditions'), 'class' => 'textfield'),
      array('data' => t('Theme'), 'class' => 'select'),
      array('data' => t('Enabled'), 'class' => 'checkbox'),
      t('Weight'),
      array('data' => t('Operations'), 'colspan' => '1'),
    );

    $output .= theme('table', $header, $rows, array('id' => 'themekey-properties'));
  }

  $rows = array();

  if (!empty($form['new_item'])) {
    if (isset($form['new_item']['property'])) {
      $row = is_array($form['new_item']['#attributes']) ? $form['new_item']['#attributes'] : array();

      $elements = array();
      $elements[] = t('New Rule:');
      $elements[] = array('data' => drupal_render($form['new_item']['property']), 'class' => 'select');
      $elements[] = array('data' => drupal_render($form['new_item']['value']), 'class' => 'textfield');
      $elements[] = array('data' => drupal_render($form['new_item']['conditions']), 'class' => 'textfield');
      $elements[] = array('data' => drupal_render($form['new_item']['theme']), 'class' => 'select');
      $elements[] = array('data' => drupal_render($form['new_item']['enabled']), 'class' => 'checkbox');

      $row['data'] = $elements;
      $rows[] = $row;
    }
  }

  if (!empty($rows)) {
    $header = array(
      '',
      array('data' => t('Property'), 'class' => 'select'),
      array('data' => t('Value'), 'class' => 'textfield'),
      array('data' => t('Conditions'), 'class' => 'textfield'),
      array('data' => t('Theme'), 'class' => 'select'),
      array('data' => t('Enabled'), 'class' => 'checkbox'),
    );

    $output .= '<a name="themekey_new_rule"></a>';
    $output .= theme('table', $header, $rows, array('id' => 'themekey-new-item'));
  }

  $output .= drupal_render($form);

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
