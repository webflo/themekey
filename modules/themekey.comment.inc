<?php
// $Id$

function themekey_comment_themekey_properties() {
  // Mapping functions
  $maps = array();
  $maps[] = array('src'       => 'cid',
                  'dst'       => 'nid',
                  'callback'  => 'themekey_comment_cid2nid');

  // Attributes for properties ;)
  $attributes = array();
  $attributes['cid'] = array('description' => t('Comment: ID'));
  
  return array('maps' => $maps, 'attributes' => $attributes);
}

function themekey_comment_themekey_paths() {
  $paths = array();
  $paths[] = array('path' => 'comment/reply/#nid');
  $paths[] = array('path' => 'comment/reply/#nid/#cid');
  $paths[] = array('path' => 'comment/edit/#cid');
  $paths[] = array('path' => 'comment/delete/#cid');
  
  return $paths;
}

function themekey_comment_cid2nid($cid) {
  $nid = db_result(db_query('SELECT nid FROM {comments} WHERE cid = %d', $cid));
  return $nid ? $nid : FALSE;
}
