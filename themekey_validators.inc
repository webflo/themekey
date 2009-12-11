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
    case '=':
    case '!':
    case '<':
    case '<=':
    case '>':
    case '>=':
      break;

    default:
      $errors['operator'] = t('Possible operators are "=", "!", "<", "<=", ">" and ">="');
  }

  if (!ctype_digit($rule['value'])) {
    $errors['value'] = t('Value must be a number');
  }

  return $errors;
}

//themekey_validator_language
//themekey_validator_date_time
//themekey_validator_date
//themekey_validator_node_type
