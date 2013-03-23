<?php
/**
 * @file
 * Definition of IPVenger\CMS\IPVengerCacheInterface.
 */

namespace IPVenger\CMS;

/**
 * Defines an interface for cache implementations.
 *
 * All cache implementations have to implement this interface.
 *
 * There is one way to "remove" a cache item:
 * - Deletion (using delete(), deleteMultiple(), deleteAll() or
 *   deleteExpired()): Permanently removes the item from the cache.
 *
 * Cache items should be deleted if they are no longer considered useful. This
 * is relevant e.g. if the cache item contains references to data that has been
 * deleted.
 */
interface IPVengerCacheInterface {

  /**
   * The default time limit for valid ipvenger cache.
   */
  const IPVENGER_CACHE_TIME_LIMIT = 360;

  /**
   * Indicates that the item should never be removed unless explicitly selected.
   *
   * The item may be removed using cache()->delete() with a cache ID.
   */
  const IPVENGER_CACHE_PERMANENT = 0;

  /**
   * Returns data from the persistent cache.
   *
   * @param string $cid
   *   The cache ID of the data to retrieve.
   * @param bool $allow_invalid
   *   (optional) If TRUE, a cache item may be returned even if it is expired.
   *   Defaults to FALSE.
   *
   * @return object|false
   *   The cache item or FALSE on failure.
   *
   * @see IPVenger\CMS\IPVengerCacheInterface::getMultiple()
   */
  public function get($cid, $allow_invalid = FALSE);

  /**
   * Returns data from the persistent cache when given an array of cache IDs.
   *
   * @param array $cids
   *   An array of cache IDs for the data to retrieve.
   * @param bool $allow_invalid
   *   (optional) If TRUE, cache items may be returned even if they have expired.
   *   Defaults to FALSE.
   *
   * @return array
   *   An array of cache item objects indexed by cache ID.
   *
   * @see IPVenger\CMS\IPVengerCacheInterface::get()
   */
  public function getMultiple(array $cids, $allow_invalid = FALSE);

  /**
   * Stores data in the persistent cache.
   *
   * @param string $cid
   *   The cache ID of the data to store.
   * @param mixed $data
   *   The data to store in the cache.
   * @param int $expire
   *   One of the following values:
   *   - IPVengerCacheInterface::IPVENGER_CACHE_PERMANENT: Indicates that the item
   *     should never be removed unless cache->delete($cid) is used explicitly.
   *   - Seconds from now: Indicates that the item will be considered invalid
   *     after this time, i.e. it will not be returned by get() unless
   *     $allow_invalid has been set to TRUE. When the item has expired, it may
   *     be permanently deleted by the garbage collector at any time.
   *
   * @see IPVenger\CMS\IPVengerCacheInterface::get()
   * @see IPVenger\CMS\IPVengerCacheInterface::getMultiple()
   */
  public function set($cid, $data, $expire = IPVENGER_CACHE_TIME_LIMIT);

  /**
   * Deletes an item from the cache.
   *
   * @param string $cid
   *   The cache ID to delete.
   *
   * @see IPVenger\CMS\IPVengerCacheInterface::deleteMultiple()
   * @see IPVenger\CMS\IPVengerCacheInterface::deleteAll()
   * @see IPVenger\CMS\IPVengerCacheInterface::deleteExpired()
   */
  public function delete($cid);

  /**
   * Deletes multiple items from the cache.
   *
   * @param array $cids
   *   An array of cache IDs to delete.
   *
   * @see IPVenger\CMS\IPVengerCacheInterface::delete()
   * @see IPVenger\CMS\IPVengerCacheInterface::deleteAll()
   * @see IPVenger\CMS\IPVengerCacheInterface::deleteExpired()
   */
  public function deleteMultiple(array $cids);

  /**
   * Deletes all cache items in a bin.
   *
   * @see IPVenger\CMS\IPVengerCacheInterface::delete()
   * @see IPVenger\CMS\IPVengerCacheInterface::deleteMultiple()
   * @see IPVenger\CMS\IPVengerCacheInterface::deleteExpired()
   */
  public function deleteAll();

  /**
   * Deletes expired items from the cache.
   *
   * @see IPVenger\CMS\IPVengerCacheInterface::delete()
   * @see IPVenger\CMS\IPVengerCacheInterface::deleteMultiple()
   * @see IPVenger\CMS\IPVengerCacheInterface::deleteAll()
   * @see IPVenger\CMS\IPVengerCacheInterface::deleteExpired()
   */
  public function deleteExpired();

}
