<?php
/**
 * NeFo Drush v1.0
 * @copyright (C) 2016 www.mfn-berlin.de
 * @author  antonio.ponte@mfn-berlin.de
 *
 * @file nefo_drush.drush.inc
 * @brief Command file for NeFo Drush
 */


/**
 * Implements hook_drush_command().
 */
function nefo_drush_drush_command() {
  $items = array();
  $items['find-institutions-with-broken-link'] = array(
    'description' => 'Find NeFo institutions with broken link.',
    'aliases' => array('fiwbl'),
    'examples' => array(
      'drush fiwbl' => 'Find NeFo institutions with broken link.',
    ),
  );

  return $items;
}

/**
 * Implements drush_hook_COMMAND().
 */
function drush_nefo_drush_find_institutions_with_broken_link() {
  $num_rows = 0;
  db_truncate('nefo__tools_broken_links')->execute();
  $result = db_query('SELECT iid, url FROM {nefo__actors_institutions} WHERE url IS NOT NULL');
  foreach ($result as $row) {
    $url = preg_replace('/\s+/', '', $row->url);
    if (!empty($url) && !nefo_check_url($url)) {
        $num_rows++;
        db_insert('nefo__tools_broken_links')
          ->fields(array('iid' => $row->iid))
          ->execute();
    }
  }
  $msg = (!$num_rows) ? 'There are no institutions with broken links.' : 'Found '. $num_rows .' institution(s) with broken links';
  watchdog('NEFO_DRUSH', $msg);
}

/**
 * Implements drush_hook_COMMAND_validate().
 */
function drush_nefo_drush_find_institutions_with_broken_link_validate() {
  $name = posix_getpwuid(posix_geteuid());
  if ($name['name'] !== 'root') {
    return drush_set_error('ACCESS_DENIED', dt('Access denied. You do not have sufficient privileges.'));
  }
}

