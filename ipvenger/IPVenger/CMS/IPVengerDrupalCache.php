<?php
/**
 * @file
 * Definition of IPVenger\CMS\IPVengerDrupalCache.
 */

namespace IPVenger\CMS;

use IPVenger\CMS\IPVengerCacheInterface;

/**
 * Defines a default cache implementation for Drupal.
 */
class IPVengerDrupalCache implements IPVengerCacheInterface {

  public function get($cid, $allow_invalid = FALSE) {
    return reset((self::getMultiple(array($cid), $allow_invalid)));
  }

  public function getMultiple(array $cids, $allow_invalid = FALSE) {
    $results = cache_get_multiple($cids, 'cache_ipvenger');

    $cache = array();
    foreach ($results as $item) {
      $item = $this->validateItem($item, $allow_invalid);
      if ($item) {
        $cache[$item->cid] = $item;
      }
    }
    $cids = array_diff($cids, array_keys($cache));
    return $cache;
  }

  public function set($cid, $data, $expire = self::IPVENGER_CACHE_TIME_LIMIT) {
    $expire = $expire != self::IPVENGER_CACHE_PERMANENT ? REQUEST_TIME + $expire : self::IPVENGER_CACHE_PERMANENT;
    cache_set($cid, $data, 'cache_ipvenger', $expire);
  }

  public function delete($cid) {
    clear($cid);
  }

  public function deleteMultiple(array $cids) {
    foreach ($cids as $cid) {
      self::delete($cid);
    }
  }

  public function deleteAll() {
    cache_clear_all('*', 'cache_ipvenger', TRUE);
  }

  public function deleteExpired() {
    db_delete('cache_ipvenger')
        ->condition('expire', CACHE_PERMANENT, '<>')
        ->condition('expire', REQUEST_TIME, '<=')
        ->execute();
  }

  /**
   * Validates a cached item.
   *
   * Checks that items are either permanent or did not expire.
   *
   * @param stdClass $cache
   *   An item loaded from cache_get() or cache_get_multiple().
   * @param bool $allow_invalid
   *   If FALSE, the method returns FALSE if the cache item is not valid.
   *
   * @return mixed|false
   *   A property indicating whether the item is valid,
   *   or FALSE if there is no valid item to load.
   */
  private function validateItem($cache, $allow_invalid) {
    if (!isset($cache->data)) {
      return FALSE;
    }

    // Check expire time.
    $cache->valid = $cache->expire == self::IPVENGER_CACHE_PERMANENT || $cache->expire >= REQUEST_TIME;

    if (!$allow_invalid && !$cache->valid) {
      return FALSE;
    }

    return $cache;
  }
}
