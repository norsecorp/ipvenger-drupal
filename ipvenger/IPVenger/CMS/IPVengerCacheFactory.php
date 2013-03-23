<?php
/**
 * @file
 * Definition of IPVenger\CMS\IPVengerCacheInterface.
 */

namespace IPVenger\CMS;

class IPVengerCacheFactory {

  /**
   * Instantiates an IPVenger cache backend class for IPVenger.
   *
   * @return IPVenger\Cache\IPVengerCacheInterface
   *   The cache object.
   */
  public static function get() {
    // Is this running on Drupal?
    if (defined('DRUPAL_CORE_COMPATIBILITY')) {
      $cache = &drupal_static(__FUNCTION__);
      if (!isset($cache)) {
        $cache = new IPVengerDrupalCache();
      }
    }

    return $cache;
  }
}
