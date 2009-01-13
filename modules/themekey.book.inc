<?php
// $Id$

function themekey_book_themekey_properties() {
  // Attributes for properties
  $attributes = array();
  $attributes['book:bid'] = array('description' => t('Book: ID'));
  $attributes['book:has_children'] = array('description' => t('Book: Has Children'));

  return array('attributes' => $attributes);
}
