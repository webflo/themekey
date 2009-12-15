<?php
// $Id$

function themekey_validator_nummeric_boolean($rule) {
  $errors = array();
  switch ($rule['operator']) {
    case '=':
    case '!':
      break;

    default:
      $errors['operator'] = t('Possible operators are "=" and "!"');
  }

  if (!(is_numeric($rule['value']) && 0 <= $rule['value'] && 1 >= $rule['value'])) {
    $errors['value'] = t('Possible values are "0" and "1"');
  }

  return $errors;
}


function themekey_validator_ctype_digit($rule) {
  $errors = array();
  switch ($rule['operator']) {
    case '~':
      $errors['operator'] = t('Possible operators are "=", "!", "<", "<=", ">" and ">="');
      break;
  }

  if (!ctype_digit($rule['value'])) {
    $errors['value'] = t('Value must be a number');
  }

  return $errors;
}


function themekey_validator_language($rule) {
  $errors = array();
  switch ($rule['operator']) {
    case '=':
      $languages = language_list();
      if (!array_key_exists($rule['value'], $languages)) {
        $errors['value'] = t('Possible values are %languages', array('%languages' => '"' . implode('", "', array_keys($languages)) . '"'));
      }
      break;

    case '~':
      $errors = themekey_validator_regex($rule);
      break;
  }

  return $errors;
}


function themekey_validator_date_time($rule) {
  $errors = array();
  switch ($rule['operator']) {
    case '=':
    case '!':
      // It seems senseless to switch a theme for one second
      $errors['operator'] = t('Possible operators are "<", "<=", ">", ">=" and "~"');
      break;

    case '~':
      $errors = themekey_validator_regex($rule);
      break;

    default:
      if (!preg_match("/^[0-9]{4}[0-9\- :]*$/", $rule['value'])) {
        $errors['value'] = t("Value isn't suitable for checks against dates formatted like \"2009-12-24 23:56:17\"");
      }
  }

  return $errors;
}


function themekey_validator_date($rule) {
  $errors = array();
  switch ($rule['operator']) {
    case '=':
    case '!':
      if (!preg_match("/^[0-9]{4}-[0-1]{1}[0-9]{1}-[0-3]{1}[0-9]{1}$/", $rule['value'])) {
        $errors['value'] = t('Not a valid date string. Format schould look like "2009-12-24"');
      }
      break;

    case '~':
      $errors = themekey_validator_regex($rule);
      break;

    default:
      if (!preg_match("/^[0-9]{4}[0-9\-]*$/", $rule['value'])) {
        $errors['value'] = t("Value isn't suitable for checks against dates formatted like \"2009-12-24\"");
      }
  }

  return $errors;
}


function themekey_validator_node_type($rule) {
  $errors = array();
  switch ($rule['operator']) {
    case '=':
      $node_types = node_get_types();
      if (!array_key_exists($rule['value'], $node_types)) {
        $errors['value'] = t('Possible values are %node_types', array('%node_types' => '"' . implode('", "', array_keys($node_types)) . '"'));
      }
      break;

    case '~':
      $errors = themekey_validator_regex($rule);
      break;

    default:
      if (!preg_match("/^[0-9a-z_]+$/", $rule['value'])) {
        $errors['value'] = t("Value isn't suitable. It must contain only lowercase letters, numbers, and underscores");
      }
  }

  return $errors;
}


function themekey_validator_http_host($rule) {
  $errors = array();
  switch ($rule['operator']) {
    case '~':
      $errors = themekey_validator_regex($rule);
      break;

    default:
      if (!preg_match("/^[0-9a-z\-.]+$/", $rule['value'])) {
        $errors['value'] = t("Value isn't suitable. It must contain only lowercase letters, numbers, and minuses");
      }
  }

  return $errors;
}


function themekey_validator_no_whitespace($rule) {
  $errors = array();
  switch ($rule['operator']) {
    case '~':
      $errors = themekey_validator_regex($rule);
      break;

    default:
      if (preg_match("/\s/", $rule['value'])) {
        $errors['value'] = t('Value could not contain white spaces');
      }
    }

  return $errors;
}


function themekey_validator_wildcard($rule) {
  $errors = themekey_validator_no_whitespace($rule);

  if (preg_match("/^[#%]/", $rule['wildcard'])) {
    $errors['wildcard'] = t('Wildcard must not start with type identifier "#" or "%" at this point');
  }
  else if (preg_match("/\s/", $rule['wildcard'])) {
    $errors['wildcard'] = t('Wildcard must not contain white spaces');
  }

  return $errors;
}


function themekey_validator_regex($rule) {
  $errors = array();
  switch ($rule['operator']) {
    case '~':
      if (FALSE === @preg_match($rule['value'], 'dummy')) {
        $errors['value'] = t('Regular expression seems to be malformed. See !link for details', array('!link' => l('PHP Manual', 'http://php.net/manual/en/pcre.pattern.php')));
      }
      break;
  }

  return $errors;
}
