<?php
// $Id$

function themekey_book_themekey_properties() {
  // Attributes for properties
  $attributes = array();
  $attributes['book:bid'] = array(
    'description' => t('Book: ID'),
    'validator' => 'themekey_validator_ctype_digit',
  );
  $attributes['book:has_children'] = array(
    'description' => t('Book: Has Children'),
    'validator' => 'themekey_validator_nummeric_boolean',
  );

  $maps = array();
  $maps[] = array('src'       => 'node:nid',
                  'dst'       => 'book:bid',
                  'callback'  => 'themekey_book_nid2bid');
  $maps[] = array('src'       => 'node:nid',
                  'dst'       => 'book:has_children',
                  'callback'  => 'themekey_book_nid2has_children');

  return array('attributes' => $attributes, 'maps' => $maps);
}

function themekey_book_nid2bid($nid, $object = NULL) {
  if ($node = node_load($nid)) {
    if (!empty($node->book['bid'])) {
      return $node->book['bid'];
    }
  }

  return NULL;
}

function themekey_book_nid2has_children($nid, $object = NULL) {
  if ($node = node_load($nid)) {
    if (!empty($node->book['has_children'])) {
      return $node->book['has_children'];
    }
  }

  return NULL;
}
